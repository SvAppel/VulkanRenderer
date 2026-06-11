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
