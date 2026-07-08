#pragma once

#include <vulkan/vulkan_raii.hpp>

/**
 * @brief Create a image view object
 * 
 * @param logicalDevice: The vulkan device
 * @param image: Image to view
 * @param format: image format
 * 
 * @return vk::raii::ImageView: The created image view
 */
vk::raii::ImageView create_image_view(vk::raii::Device& logicalDevice, vk::Image image, vk::Format format);

void transition_image_layout(vk::raii::CommandBuffer& commandBuffer, vk::Image image,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
    vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
    vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage);