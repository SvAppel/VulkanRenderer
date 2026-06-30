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

	std::cout << "Physical Device name: " << properties.deviceName << std::endl;

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

void Logger::log(const vk::SurfaceCapabilitiesKHR& capabilities)
{
	if(!enabled)
		return;

	std::cout << "Swapchain can support the following surface capabilities:" << std::endl;

	std::cout << "\tMinimum image count: " << capabilities.minImageCount << std::endl;
	std::cout << "\tMaximum image count: " << capabilities.maxImageCount << std::endl;

	std::cout << "\tCurrent extent: \tWidth: " << capabilities.currentExtent.width << " Height: " << capabilities.currentExtent.height << std::endl;
	std::cout << "\tMinimum extent: \tWidth: " << capabilities.minImageExtent.width << " Height: " << capabilities.minImageExtent.height << std::endl;
	std::cout << "\tMaximum extent: \tWidth: " << capabilities.maxImageExtent.width << " Height: " << capabilities.maxImageExtent.height << std::endl;

	std::cout << "\tMaximum image array layers: " << capabilities.maxImageArrayLayers << std::endl;

	std::cout << "\tSupported transforms:" << std::endl;
	std::vector<std::string> stringList = parse_transform_bits(capabilities.supportedTransforms);
	log(stringList, "\t\t");

	std::cout << "\tSupported alpha operations:" << std::endl;
	stringList = parse_alpha_composite_bits(capabilities.supportedCompositeAlpha);
	log(stringList, "\t\t");

	std::cout << "\tSupported image usage:" << std::endl;
	stringList = parse_image_usage_bits(capabilities.supportedUsageFlags);
	log(stringList, "\t\t");
}

void Logger::log(std::vector<vk::SurfaceFormatKHR> formats)
{
	if(!enabled)
		return;

	std::cout << "There are " << formats.size() << " surface formats supported:" << std::endl;

	for(uint32_t i = 0; i < formats.size(); i++)
	{
		std::cout << "\tFormat " << i << ": "
			<< vk::to_string(formats[i].format) << ", ColorSpace: "
			<< vk::to_string(formats[i].colorSpace) << std::endl;
	}
}

void Logger::log(std::vector<vk::PresentModeKHR> presentModes)
{
	if(!enabled)
		return;

	std::cout << "There are " << presentModes.size() << " present modes supported:" << std::endl;

	for(uint32_t i = 0; i < presentModes.size(); i++)
	{
		std::cout << "\tPresent mode " << i << ": " << vk::to_string(presentModes[i]) << std::endl;
	}
}

void Logger::log(std::vector<std::string>& entries, const std::string& indent)
{
	if(!enabled)
		return;

	for(const std::string& entry : entries)
	{
		std::cout << indent << entry << std::endl;
	}
}

std::vector<std::string> Logger::parse_transform_bits(vk::SurfaceTransformFlagsKHR bits)
{
	std::vector<std::string> result;
	if(bits & vk::SurfaceTransformFlagBitsKHR::eIdentity)
		result.push_back("Identity");
	if(bits & vk::SurfaceTransformFlagBitsKHR::eRotate90)
		result.push_back("Rotate90");
	if(bits & vk::SurfaceTransformFlagBitsKHR::eRotate180)
		result.push_back("Rotate180");
	if(bits & vk::SurfaceTransformFlagBitsKHR::eRotate270)
		result.push_back("Rotate270");
	if(bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror)
		result.push_back("HorizontalMirror");
	if(bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90)
		result.push_back("HorizontalMirrorRotate90");
	if(bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180)
		result.push_back("HorizontalMirrorRotate180");
	if(bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270)
		result.push_back("HorizontalMirrorRotate270");
	return result;
}

std::vector<std::string> Logger::parse_alpha_composite_bits(vk::CompositeAlphaFlagsKHR bits)
{
	std::vector<std::string> result;
	if(bits & vk::CompositeAlphaFlagBitsKHR::eOpaque)
		result.push_back("Opaque");
	if(bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
		result.push_back("PreMultiplied");
	if(bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
		result.push_back("PostMultiplied");
	if(bits & vk::CompositeAlphaFlagBitsKHR::eInherit)
		result.push_back("Inherit");
	return result;
}

std::vector<std::string> Logger::parse_image_usage_bits(vk::ImageUsageFlags bits)
{
	std::vector<std::string> result;
	if(bits & vk::ImageUsageFlagBits::eTransferSrc)
		result.push_back("TransferSrc");
	if(bits & vk::ImageUsageFlagBits::eTransferDst)
		result.push_back("TransferDst");
	if(bits & vk::ImageUsageFlagBits::eSampled)
		result.push_back("Sampled");
	if(bits & vk::ImageUsageFlagBits::eStorage)
		result.push_back("Storage");
	if(bits & vk::ImageUsageFlagBits::eColorAttachment)
		result.push_back("ColorAttachment");
	if(bits & vk::ImageUsageFlagBits::eDepthStencilAttachment)
		result.push_back("DepthStencilAttachment");
	if(bits & vk::ImageUsageFlagBits::eTransientAttachment)
		result.push_back("TransientAttachment");
	if(bits & vk::ImageUsageFlagBits::eInputAttachment)
		result.push_back("InputAttachment");
	return result;
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
