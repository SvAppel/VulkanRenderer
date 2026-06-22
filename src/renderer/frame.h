#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include "image.h"

/**
 * @brief Holds all the state used in one rendering/presentation operation.
 */
class Frame
{
public:
    Frame(vk::Image image, vk::raii::Device& logicalDevice, vk::Format swapchainFormat);

    /**
     * @brief Set (and record) the command buffer
     * 
     * @param newCommandBuffer: The command buffer to record to
     * @param shaders: Shader objects to use
     * @param framesize: size of the screen
     */
    void set_command_buffer(vk::raii::CommandBuffer& newCommandBuffer, std::vector<vk::raii::ShaderEXT>& shaders, vk::Extent2D frameSize);

    /**
     * @brief Allocate command buffer
     * 
     * @param logicalDevice: The vulkan device
     * @param commandPool: The command pool to allocate from
     * 
     * @return The allocated command buffer
     */
    vk::raii::CommandBuffer allocate_command_buffer(vk::raii::Device& logicalDevice, vk::raii::CommandPool commandPool);

    /**
     * @brief Swapchain image to render to
     */
    vk::Image image;

    /**
     * @brief View of the swapchain image
     */
    vk::raii::ImageView imageView = nullptr;

    /**
     * @brief For recording drawing commands
     */
    vk::raii::CommandBuffer commandBuffer = nullptr;

private:

    /**
     * @brief Build a description of the rendering info
     * 
     * @param frameSize: Size of the screen
     */
    void build_rendering_info(vk::Extent2D frameSize);

    /**
     * @brief Build a description of the color attachment
     */
    void build_color_attachment();

    /**
     * @brief Dynamically set the normally static pipeline parts within the command buffer
     * 
     * @param frameSize: Size of the screen
     */
    void set_dynamic_states(vk::Extent2D frameSize);

    vk::RenderingInfoKHR renderingInfo = {};

    vk::RenderingAttachmentInfoKHR colorAttachment = {};
};