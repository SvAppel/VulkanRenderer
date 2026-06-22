#include "renderer.h"
#include "device.h"
#include "shader.h"
#include "command.h"

#include <iostream>
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

	physicalDevice = choose_physical_device(instance);
	logicalDevice = create_logical_device(physicalDevice, surface);
	uint32_t graphicsQueueFamilyIndex = findQueueFamilyIndex(physicalDevice, surface, vk::QueueFlagBits::eGraphics);
	graphicsQueue = logicalDevice.getQueue(graphicsQueueFamilyIndex, 0);

	int width, height;
	glfwGetWindowSize(window, &width, &height);
	swapchain.build(logicalDevice, physicalDevice, surface, width, height);

	std::vector<vk::Image> swapchainImages = swapchain.chain.getImages();

	for(uint32_t i = 0; i<swapchainImages.size(); i++)
	{
		frames.push_back(Frame(swapchainImages[i], logicalDevice, swapchain.format.format));
	}

	shaders = make_shader_objects(logicalDevice, "../shaders/vertex.spv", "../shaders/fragment.spv");

	commandPool = make_command_pool(logicalDevice, graphicsQueueFamilyIndex);

	for(uint32_t i = 0; i < swapchainImages.size(); i++)
	{
		vk::raii::CommandBuffer commandBuffer = allocate_command_buffer(logicalDevice, commandPool);
		frames[i].set_command_buffer(commandBuffer, shaders, swapchain.extent);
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
	uint32_t imageIndex{0};

	vk::SubmitInfo submitInfo
	{
		.commandBufferCount = 1,
		.pCommandBuffers = &*frames[0].commandBuffer
	};

	graphicsQueue.submit(submitInfo);
	graphicsQueue.waitIdle();

	vk::PresentInfoKHR presentInfo
	{
		.swapchainCount = 1,
		.pSwapchains = &*swapchain.chain,
		.pImageIndices = &imageIndex
	};

	vk::Result result = graphicsQueue.presentKHR(presentInfo);
	if(result != vk::Result::eSuccess)
		logger->print_error("Unable to present image!");

	graphicsQueue.waitIdle();
}