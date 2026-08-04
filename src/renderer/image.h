#pragma once

#include <vulkan/vulkan_raii.hpp>

vk::raii::Image make_image
(
    vk::raii::Device& logicalDevice, 
    uint32_t width, uint32_t height,
    vk::ImageTiling tiling, 
    vk::ImageUsageFlags usage, 
    vk::MemoryPropertyFlags memoryProperties
);

vk::raii::DeviceMemory make_image_memory
(
    vk::raii::PhysicalDevice& physicalDevice, 
    vk::raii::Device& logicalDevice,
    vk::MemoryPropertyFlags memoryProperties, 
    vk::raii::Image& image
);

/**
 * @brief Create a image view object
 * 
 * @param logicalDevice: The vulkan device
 * @param image: Image to view
 * @param format: image format
 * 
 * @return vk::raii::ImageView: The created image view
 */
vk::raii::ImageView make_image_view(vk::raii::Device& logicalDevice, vk::Image image, vk::Format format);

void copy_buffer_to_image(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height);

void transition_image_layout(vk::raii::CommandBuffer& commandBuffer, vk::Image image,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
    vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
    vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage);