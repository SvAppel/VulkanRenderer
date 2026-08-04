#include "renderer.h"
#include "device.h"
#include "shader.h"
#include "command.h"
#include "synchronisation.h"
#include "descriptors.h"

#include <iostream>
#include <memory>
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE; // In a .cpp file
//#include <vulkan/vulkan_raii.hpp>

Engine::Engine(GLFWwindow* window) : window(window)
{
	logger = Logger::get_logger();
	logger->print("Made a graphics engine.");

	make_instance("Real Engine", deletionQueue);
	//dldi = vk::detail::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
	debugMessenger = logger->make_debug_messenger(instance/*, dldi*/);

	VkSurfaceKHR raw_surface;
	if(glfwCreateWindowSurface(*instance, window, nullptr, &raw_surface) != 0)
		logger->print_error("Failed to create window surface!");
	surface = vk::raii::SurfaceKHR(instance, raw_surface);
	logger->print("Successfully created vulkan surface!");

	// The properties of the physical device are very verbose
	logger->set_mode(false);
	physicalDevice = choose_physical_device(instance);
	logger->set_mode(true);
	logicalDevice = create_logical_device(physicalDevice, surface);
	uint32_t graphicsQueueFamilyIndex = findQueueFamilyIndex(physicalDevice, surface, vk::QueueFlagBits::eGraphics);
	graphicsQueue = logicalDevice.getQueue(graphicsQueueFamilyIndex, 0);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	swapchain.build(logicalDevice, physicalDevice, surface, width, height);

	// Create per Frame Layouts and Pools
	DescriptorManager descriptorManager(logicalDevice);
	descriptorManager.add_descriptor(vk::ShaderStageFlagBits::eVertex, vk::DescriptorType::eUniformBuffer);
	frameDescriptorSetLayout = descriptorManager.build_layout();

	std::vector<vk::DescriptorType> frameDescriptorTypes = {vk::DescriptorType::eUniformBuffer};
	frameDescriptorPool = descriptorManager.make_descriptor_pool(MAX_FRAMES_IN_FLIGHT, frameDescriptorTypes.size(), frameDescriptorTypes.data());

	// Create Texture Layouts and Pools
	descriptorManager.add_descriptor(vk::ShaderStageFlagBits::eFragment, vk::DescriptorType::eCombinedImageSampler);
	meshDescriptorSetLayout = descriptorManager.build_layout();

	std::vector<vk::DescriptorType> meshDescriptorTypes = {vk::DescriptorType::eCombinedImageSampler};
	//For descriptorSetCount: how many textures do we have?
	meshDescriptorPool = descriptorManager.make_descriptor_pool(1, meshDescriptorTypes.size(), meshDescriptorTypes.data());

	PipelineLayoutManager pipelineLayoutManager(logicalDevice);
	pipelineLayoutManager.add_push_constant(vk::ShaderStageFlagBits::eVertex, sizeof(ObjectDataConstants));
	pipelineLayoutManager.add(frameDescriptorSetLayout);
	pipelineLayoutManager.add(meshDescriptorSetLayout);
	pipelineLayout = pipelineLayoutManager.build_layout();

	//The logger would print the preprocessed and compiled shader code
	logger->set_mode(false);
	std::vector<vk::DescriptorSetLayout> descriptorLayouts = {frameDescriptorSetLayout, meshDescriptorSetLayout};
	shaders = make_shader_objects(logicalDevice, "shader", descriptorLayouts, pipelineLayoutManager.pushConstants);
	logger->set_mode(true);

	commandPool = make_command_pool(logicalDevice, graphicsQueueFamilyIndex);

	mesh = build_mesh(physicalDevice, logicalDevice, commandPool, graphicsQueue);

	//TODO: make new or use existing commandbuffer, replace frameDescriptorSetLayout and frameDescriptorPool with the proper ones
	material = std::make_unique<Texture>(physicalDevice, logicalDevice, commandPool, graphicsQueue, meshDescriptorSetLayout, meshDescriptorPool, "../textures/benrath.jpg");

	for(uint32_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++)
	{
		vk::raii::CommandBuffer commandBuffer = allocate_command_buffer(logicalDevice, commandPool);
		frames.push_back(Frame(swapchain, physicalDevice, logicalDevice, shaders, commandBuffer, frameDescriptorSetLayout, frameDescriptorPool, pipelineLayout, &mesh, material.get()));
	}
}

Engine::~Engine()
{
	logger->print("Destroying Engine!");

	while (deletionQueue.size() > 0) 
	{
		deletionQueue.back()();
		deletionQueue.pop_back();
	}
}

bool Engine::supported_by_instance(const char** extensionNames, int extensionCount, const char** layerNames, int layerCount)
{
	Logger* logger = Logger::get_logger();
	std::stringstream lineBuilder;

	/*
	* Check extension support
	*/
	std::vector<vk::ExtensionProperties> supportedExtensions = vk::enumerateInstanceExtensionProperties();

	logger->print("Instance can support the following extensions:");
	logger->print_extensions(supportedExtensions);

	bool found;
	for(int i = 0; i < extensionCount; i++)
	{
		const char* extension = extensionNames[i];
		found = false;
		for(VkExtensionProperties supportedExtension: supportedExtensions)
		{
			if(strcmp(extension, supportedExtension.extensionName) == 0)
			{
				found = true;
				lineBuilder << "Extension \"" << extension << "\" is supported!";
				logger->print(lineBuilder.str());
				lineBuilder.str("");
				break;
			}
		}

		if(!found)
		{
			lineBuilder << "Extension \"" << extension << "\" is not supported!";
			logger->print_error(lineBuilder.str());
			return false;
		}
	}

	/*
	* Check layer support
	*/
	std::vector<vk::LayerProperties> supportedLayers = vk::enumerateInstanceLayerProperties();

	logger->print("Instance can support the following layers:");
	logger->print_layers(supportedLayers);

	for(int i = 0; i < layerCount; i++)
	{
		const char* layer = layerNames[i];
		found = false;
		for(VkLayerProperties supportedLayer: supportedLayers)
		{
			if(strcmp(layer, supportedLayer.layerName) == 0)
			{
				found = true;
				lineBuilder << "Layer \"" << layer << "\" is supported!";
				logger->print(lineBuilder.str());
				lineBuilder.str("");
				break;
			}
		}

		if(!found)
		{
			lineBuilder << "Layer \"" << layer << "\" is not supported!";
			logger->print_error(lineBuilder.str());
			return false;
		}
	}

	return true;
}

void Engine::make_instance(const char* applicationName, std::deque<std::function<void()>>& deletionQueue)
{
	Logger* logger = Logger::get_logger();

	logger->print("Making an instance...");

	uint32_t version = vk::enumerateInstanceVersion();

	logger->report_version_number(version);

	vk::ApplicationInfo appInfo{
      .pApplicationName = applicationName,
      .applicationVersion = version,
      .pEngineName = NULL,
      .engineVersion = version,
      .apiVersion = version
    };

	/*
	* Extensions
	*/
	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
	uint32_t enabledExtensionCount = glfwExtensionCount;

	if(logger->is_enabled())
		enabledExtensionCount++;

	const char** ppEnabledExtensionNames = (const char**)malloc(enabledExtensionCount * sizeof(const char*));

	for(int i = 0; i < glfwExtensionCount; i++)
	{
		ppEnabledExtensionNames[i] = glfwExtensions[i];
	}

	if(logger->is_enabled())
		ppEnabledExtensionNames[glfwExtensionCount] = VK_EXT_DEBUG_UTILS_EXTENSION_NAME;
	
	logger->print("Extensions to be requested:");
	logger->print_list(ppEnabledExtensionNames, enabledExtensionCount);

	/*
	* Layers
	*/
	uint32_t enabledLayerCount = 0;
	
	if(logger->is_enabled())
		enabledLayerCount++;

	const char** ppEnabledLayerNames = nullptr;
	if(enabledLayerCount > 0)
		ppEnabledLayerNames = (const char**)malloc(enabledLayerCount * sizeof(const char*));

	if(logger->is_enabled())
		ppEnabledLayerNames[0] = "VK_LAYER_KHRONOS_validation";
	
	logger->print("Layers to be requested:");
	logger->print_list(ppEnabledLayerNames, enabledLayerCount);

	if(!supported_by_instance(ppEnabledExtensionNames, enabledExtensionCount, ppEnabledLayerNames, enabledLayerCount))
		return;

	vk::InstanceCreateInfo createInfo{ 
		.pApplicationInfo = &appInfo,
		.enabledLayerCount = enabledLayerCount,
		.ppEnabledLayerNames = ppEnabledLayerNames,
		.enabledExtensionCount = enabledExtensionCount,
		.ppEnabledExtensionNames = ppEnabledExtensionNames};

	instance = vk::raii::Instance(context, createInfo);
}

void Engine::draw()
{
	Frame& frame = frames[frameIndex];

	vk::Result waitResult = logicalDevice.waitForFences(*frame.renderFinishedFence, false, UINT32_MAX);
	if(waitResult == vk::Result::eErrorOutOfDateKHR || waitResult == vk::Result::eSuboptimalKHR)
	{
		swapchain.rebuild(logicalDevice, physicalDevice, surface, window);
	}

	frame.update_uniform_buffer();

	logicalDevice.resetFences(*frame.renderFinishedFence);

	uint32_t imageIndex = swapchain.chain.acquireNextImage(UINT32_MAX, frame.imageAuqiredSemaphore).value;
	frame.record_command_buffer(imageIndex);

	vk::PipelineStageFlags waitDestinationStageMask( vk::PipelineStageFlagBits::eColorAttachmentOutput );
	vk::SubmitInfo submitInfo
	{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*frame.imageAuqiredSemaphore,
		.pWaitDstStageMask = &waitDestinationStageMask,
		.commandBufferCount = 1,
		.pCommandBuffers = &*frame.commandBuffer,
		.signalSemaphoreCount =  1,
		.pSignalSemaphores = &*frame.renderFinishedSemaphore	
	};

	graphicsQueue.submit(submitInfo, frame.renderFinishedFence);

	vk::PresentInfoKHR presentInfo
	{
		.waitSemaphoreCount = 1,
		.pWaitSemaphores = &*frame.renderFinishedSemaphore,
		.swapchainCount = 1,
		.pSwapchains = &*swapchain.chain,
		.pImageIndices = &imageIndex
	};

	vk::Result result = graphicsQueue.presentKHR(presentInfo);
	if(result == vk::Result::eErrorOutOfDateKHR || result == vk::Result::eSuboptimalKHR)
	{
		swapchain.rebuild(logicalDevice, physicalDevice, surface, window);
	}

	frameIndex = (frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
}