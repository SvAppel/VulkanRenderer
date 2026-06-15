#include "logger.h"

#include <iostream>

Logger* Logger::logger;

VKAPI_ATTR vk::Bool32 VKAPI_CALL debugCallback(
	vk::DebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	vk::DebugUtilsMessageTypeFlagsEXT messageType,
	const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	std::cerr << "Validation layer: " << pCallbackData->pMessage << std::endl;

	return vk::False;
}

Logger* Logger::get_logger()
{
	if (!logger)
		logger = new Logger();

	return logger;
}

void Logger::set_mode(bool mode)
{
	enabled = mode;
}

bool Logger::is_enabled()
{
	return enabled;
}

void Logger::report_version_number(uint32_t version)
{
	if (!enabled)
		return;

	std::cout << "System can support Vulkan variant: " << vk::apiVersionMajor(version)
		<< ", Major: " << vk::apiVersionMajor(version)
		<< ", Minor: " << vk::apiVersionMinor(version)
		<< ", Patch: " << vk::apiVersionPatch(version) << std::endl;

}

void Logger::print(std::string message)
{
	if (!enabled)
		return;

	std::cout << message << std::endl;
}

void Logger::print_list(const char** list, uint32_t count)
{
	if (!enabled)
		return;

	for (uint32_t i = 0; i < count; i++) 
	{
		std::cout << "\t\"" << list[i] << "\"" << std::endl;
	}
}

void Logger::print_extensions(std::vector<vk::ExtensionProperties>& extensions)
{
	if(!enabled)
		return;

	for(vk::ExtensionProperties extension: extensions)
	{
		std::cout << "\t\'" << extension.extensionName << "\'" << std::endl;
	}

}

void Logger::print_layers(std::vector<vk::LayerProperties>& layers)
{
	if(!enabled)
		return;

	for(vk::LayerProperties layer: layers)
	{
		std::cout << "\t\'" << layer.layerName << "\'" << std::endl;
	}
}

void Logger::print_error(std::string errorMessage)
{
	if (!enabled)
		return;

	throw std::runtime_error(errorMessage);
}

void Logger::log(const vk::raii::PhysicalDevice& device)
{
	if(!enabled)
		return;

	vk::PhysicalDeviceProperties properties = device.getProperties();

	std::cout << "Device name: " << properties.deviceName << std::endl;

	std::cout << "Device type: " << vk::to_string(properties.deviceType) << std::endl;
}

void Logger::log(std::vector<vk::QueueFamilyProperties> queueFamilies)
{
	if(!enabled)
		return;
	
	std::cout << "There are " << queueFamilies.size() << " queue families on the system" << std::endl;

	for(uint32_t i = 0; i < queueFamilies.size(); i++)
	{
		vk::QueueFamilyProperties queueFamily = queueFamilies[i];

		std::cout << "Queue family " << i << ":" << std::endl;
		std::cout << "\tSupports ";
		if(queueFamily.queueFlags & vk::QueueFlagBits::eCompute)
			std::cout << "compute, ";
		if(queueFamily.queueFlags & vk::QueueFlagBits::eGraphics)
			std::cout << "graphics, ";
		if(queueFamily.queueFlags & vk::QueueFlagBits::eTransfer)
			std::cout << "transfer, ";
		if(queueFamily.queueFlags & vk::QueueFlagBits::eOpticalFlowNV)
			std::cout << "nvidia optical flow, ";
		if(queueFamily.queueFlags & vk::QueueFlagBits::eSparseBinding)
			std::cout << "spase binding, ";
		if(queueFamily.queueFlags & vk::QueueFlagBits::eProtected)
			std::cout << "protected memory, ";
		if(queueFamily.queueFlags & vk::QueueFlagBits::eDataGraphARM)
			std::cout << "ARM data graph, ";
		if(queueFamily.queueFlags & vk::QueueFlagBits::eVideoDecodeKHR)
			std::cout << "video decode, ";
		if(queueFamily.queueFlags & vk::QueueFlagBits::eVideoEncodeKHR)
			std::cout << "video encode, ";
		
		std::cout << std::endl;

		std::cout << "\tFamily supports " << queueFamily.queueCount << " queues" << std::endl;
	}
}





vk::raii::DebugUtilsMessengerEXT Logger::make_debug_messenger(vk::raii::Instance& instance/*, vk::detail::DispatchLoaderDynamic& dldi*/)
{
	if(!enabled)
		return nullptr;
	
	vk::DebugUtilsMessageSeverityFlagsEXT severityFlags(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
		                                                    vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
	vk::DebugUtilsMessageTypeFlagsEXT     messageTypeFlags(
            vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
	vk::DebugUtilsMessengerCreateInfoEXT createInfo{.messageSeverity = severityFlags,
		                                                                      .messageType     = messageTypeFlags,
		                                                                      .pfnUserCallback = &debugCallback};

	vk::raii::DebugUtilsMessengerEXT messenger = instance.createDebugUtilsMessengerEXT(createInfo);

	return messenger;
}
