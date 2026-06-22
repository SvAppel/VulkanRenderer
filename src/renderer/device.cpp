#include "device.h"

#include "../logging/logger.h"

#include <vector>

bool supports 
(
    const vk::raii::PhysicalDevice& device,
    const char** ppRequestedExtensions,
    const uint32_t requestedExtensionCount
)
{
    Logger* logger = Logger::get_logger();
    logger->print("Requested Physical Device extensions:");
    logger->print_list(ppRequestedExtensions, requestedExtensionCount);

    std::vector<vk::ExtensionProperties> extensions = device.enumerateDeviceExtensionProperties();
    logger->print("Physical Device supported extensions: ");
    logger->print_extensions(extensions);

    for(uint32_t i = 0; i < requestedExtensionCount; i++)
    {
        bool supported = false;

        for(vk::ExtensionProperties extension: extensions)
        {
            std::string name = extension.extensionName;

            if(!name.compare(ppRequestedExtensions[i]))
            {
                supported = true;
                break;
            }
        }

        if(!supported)
            return false;
    }


    return true;
}

bool is_suitable(vk::raii::PhysicalDevice& device)
{
    Logger* logger = Logger::get_logger();
    logger->print("Checking if device is suitable");

    const char* ppRequestedExtension = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

    if(supports(device, &ppRequestedExtension, 1))
    {
        logger->print("Device can support the requested extensions!");
    }
    else
    {
        logger->print("Device can NOT support she requested extensions!");
        return false;
    }

    return true;
}

vk::raii::PhysicalDevice choose_physical_device(const vk::raii::Instance& instance)
{
    Logger* logger = Logger::get_logger();
    logger->print("Choosing physical device...");

    std::vector<vk::raii::PhysicalDevice> availableDevices = instance.enumeratePhysicalDevices();

    for(vk::raii::PhysicalDevice device: availableDevices)
    {
        logger->log(device);
        if(is_suitable(device))
            return device;
    }

    return nullptr;
}

uint32_t findQueueFamilyIndex(vk::raii::PhysicalDevice physicalDevice, vk::raii::SurfaceKHR& surface, vk::QueueFlags queueType)
{
    Logger* logger = Logger::get_logger();

    std::vector<vk::QueueFamilyProperties> queueFamilies = physicalDevice.getQueueFamilyProperties();
    logger->log(queueFamilies);

    for(uint32_t i = 0; i< queueFamilies.size(); i++)
    {
        vk::QueueFamilyProperties queueFamily = queueFamilies[i];
        bool canPresent = false;

        if(surface != nullptr)
        {
            if(physicalDevice.getSurfaceSupportKHR(i, *surface))
                canPresent = true;

        }
        else 
        {
            canPresent = true;
        }

        bool supported = false;
        if(queueFamily.queueFlags & queueType)
            supported = true;

        if(supported && canPresent)
            return i;
    }

    return UINT32_MAX;
}

vk::raii::Device create_logical_device(vk::raii::PhysicalDevice physicalDevice, vk::raii::SurfaceKHR& surface)
{
    Logger* logger = Logger::get_logger();
    logger->print("Abstracting physical GPU to logical device!");

    uint32_t graphicsIndex = findQueueFamilyIndex(physicalDevice, surface, vk::QueueFlagBits::eGraphics);
    float queuePriority = 1.0f;

    vk::DeviceQueueCreateInfo queueInfo{
        .queueFamilyIndex = graphicsIndex,
        .queueCount = 1,
        .pQueuePriorities = &queuePriority
    };

    vk::PhysicalDeviceFeatures deviceFeatures;

    vk::StructureChain<vk::PhysicalDeviceFeatures2,
                        vk::PhysicalDeviceVulkan11Features,
                        vk::PhysicalDeviceVulkan13Features,
                        vk::PhysicalDeviceExtendedDynamicStateFeaturesEXT,
                        vk::PhysicalDeviceShaderObjectFeaturesEXT>
        featureChain = {
            {},                                 // vk::PhysicalDeviceFeatures2 (empty for now)
            {.shaderDrawParameters = true},     // Enable shader draw parameters from Vulkan 1.1
            {.dynamicRendering = true},         // Enable dynamic rendering from Vulkan 1.3
            {.extendedDynamicState = true},      // Enable extended dynamic state from the extension
            {.shaderObject = true}
        };

    std::vector<const char*> requiredDeviceExtension = {
        vk::KHRSwapchainExtensionName,
        vk::EXTShaderObjectExtensionName,
        vk::KHRDynamicRenderingExtensionName
    };

    //Modern Vulkan no longer differentiates between instance and device-specific validation layers
    // std::vector<const char*> requiredLayers;
    // if(logger->is_enabled())
    //     requiredLayers.push_back("VK_LAYER_KHRONOS_validation");

    vk::DeviceCreateInfo deviceCreateInfo{
        .pNext = &featureChain.get<vk::PhysicalDeviceFeatures2>(),
        .queueCreateInfoCount = 1,
        .pQueueCreateInfos = &queueInfo,
        // .enabledLayerCount = static_cast<uint32_t>(requiredLayers.size()),
        // .ppEnabledLayerNames = requiredLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(requiredDeviceExtension.size()),
        .ppEnabledExtensionNames = requiredDeviceExtension.data()
    };

    vk::raii::Device device = vk::raii::Device(physicalDevice, deviceCreateInfo);

    logger->print("Successfully abstracted GPU!");

    return device;
}