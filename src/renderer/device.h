#pragma once

#include <vulkan/vulkan_raii.hpp>

/**
 * @brief Checks whether the physical device can support 
 * the requested extensions
 * 
 * @param device: Physcial device to check
 * @param pprequestedExtensions: Requested extension names
 * @param requestedExtensionCount: Number of requested extensions
 * 
 * @return true, if all are supported
 * @return false, otherwise
 */
bool supports 
(
    const vk::raii::PhysicalDevice& device,
    const char** ppRequestedExtensions,
    const uint32_t requestedExtensionCount
);

/**
 * @brief Checks whether the given device is suitable
 * 
 * @param device: Physical device to check
 * 
 * @return true, physical device is suitable
 * @return false, physical device is unsuitable
 */
bool is_suitable(vk::raii::PhysicalDevice& device);

/**
 * @brief Chooses a physical device for use
 * 
 * @param instance: Vulkan instance which will use the device
 * 
 * @return vk::raii::PhysicalDevice: The selected physical device
 */
vk::raii::PhysicalDevice choose_physical_device(const vk::raii::Instance& instance);

/**
 * @brief Query the given physical device for the index of
 * a queue family.
 * 
 * @param physicalDevice: Physical device to query
 * @param queueType: Type of queue being queried
 * 
 * @return uint32_t: The index of the queue family. 
 * max upon failure
 */
uint32_t findQueueFamilyIndex(vk::raii::PhysicalDevice physicalDevice, vk::raii::SurfaceKHR& surface, vk::QueueFlags queueType);

/**
 * @brief Create a logical device object
 * 
 * @param physicalDevice: Physical device to be abtracted
 * 
 * @return vk::raii:Device: An abstraction of the physical Device
 */

 vk::raii::Device create_logical_device(vk::raii::PhysicalDevice physicalDevice, vk::raii::SurfaceKHR& surface);