#include "instance.h"
#include "../logging/logger.h"
#include <GLFW/include/GLFW/glfw3.h>

vk::Instance make_instance(const char* applicationName, std::deque<std::function<void()>>& deletionQueue)
{
	Logger* logger = Logger::get_logger();

	logger->print("Making an instance...");

	uint32_t version = vk::enumerateInstanceVersion().value;

	logger->report_version_number(version);

	vk::ApplicationInfo appInfo = vk::ApplicationInfo(applicationName, version, NULL, version, version, nullptr);

	uint32_t glfwExtensionCount = 0;
	const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

	logger->print_list(glfwExtensions, glfwExtensionCount);

	vk::InstanceCreateInfo createInfo = vk::InstanceCreateInfo(vk::InstanceCreateFlags(), &appInfo, 0, nullptr, glfwExtensionCount, glfwExtensions);
	vk::ResultValue<vk::Instance> instanceAttempt = vk::createInstance(createInfo);

	if (instanceAttempt.result != vk::Result::eSuccess) 
	{
		logger->print("Failed to create Instance!");
		return nullptr;
	}

	vk::Instance instance = instanceAttempt.value;
	VkInstance handle = instance;

	deletionQueue.push_back([logger, handle]() 
		{
			vkDestroyInstance(handle, nullptr);
			logger->print("Deleted Instance!");
		});


	return instance;
}
