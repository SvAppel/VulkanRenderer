#include "renderer.h"

//VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE; // In a .cpp file
#include <vulkan/vulkan_raii.hpp>

Engine::Engine(GLFWwindow* window) : window(window)
{
	logger = Logger::get_logger();
	logger->print("Made a graphics engine.");

	//make_instance("Real Engine", deletionQueue);
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

void Engine::make_instance(const char* applicationName, std::deque<std::function<void()>>& deletionQueue)
{
	Logger* logger = Logger::get_logger();

	logger->print("Making an instance...");

	uint32_t version = vk::enumerateInstanceVersion().value;

	logger->report_version_number(version);

	//vk::ApplicationInfo appInfo = vk::ApplicationInfo(applicationName, version, NULL, version, version, nullptr);
	vk::ApplicationInfo appInfo{ 
		.pApplicationName = applicationName,
		.applicationVersion = version,
		.pEngineName = NULL,
		.engineVersion = version,
		.apiVersion = version };

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	logger->print_list(glfwExtensions, glfwExtensionCount);

	//vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(vk::InstanceCreateFlags(), &appInfo, 0, nullptr, glfwExtensionCount, glfwExtensions);
	vk::InstanceCreateInfo createInfo{ 
		.pApplicationInfo = &appInfo,
		.enabledExtensionCount = glfwExtensionCount,
		.ppEnabledExtensionNames = glfwExtensions};

	/*vk::ResultValue<vk::Instance> instanceAttempt = vk::createInstance(createInfo);

	if (instanceAttempt.result != vk::Result::eSuccess)
	{
		logger->print("Failed to create Instance!");
		return;
	}*/

	//vk::Instance instance = instanceAttempt.value;
	//vk::raii::Instance instance( context, createInfo );
	instance = vk::raii::Instance(context, createInfo);
	/*VkInstance handle = instance;

	deletionQueue.push_back([logger, handle]()
		{
			vkDestroyInstance(handle, nullptr);
			logger->print("Deleted Instance!");
		});*/


	//return instance;
}
