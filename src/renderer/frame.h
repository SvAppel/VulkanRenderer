#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "image.h"
#include "swapchain.h"
#include "../factories/mesh_factory.h"

/**
 * @brief Holds all the state used in one rendering/presentation operation.
 */
class Frame
{
public:
    Frame
    (
        Swapchain& swapchain, 
        vk::raii::Device& logicalDevice, 
        std::vector<vk::raii::ShaderEXT>& shaders, 
        vk::raii::CommandBuffer& commandBuffer,
        Mesh* triangleMesh
    );

    /**
     * @brief Set (and record) the command buffer
     * 
     * @param imageIndex: The index of the current image
     */
    void record_command_buffer(uint32_t imageIndex);

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
     * @brief For recording drawing commands
     */
    vk::raii::CommandBuffer commandBuffer = nullptr;


    Swapchain& swapchain;

    std::vector<vk::ShaderEXT> rawShaders;

    /**
	 * @brief A Semaphore for GPU synchronisation after the image has been aquired
	 */
	vk::raii::Semaphore imageAuqiredSemaphore = nullptr;

	/**
	 * @brief A Semaphore for GPU synchronisation after the render has finished
	 */
	vk::raii::Semaphore renderFinishedSemaphore = nullptr;

	/**
	 * @brief A fence for CPU synchronisation after the render has finished
	 */
	vk::raii::Fence renderFinishedFence = nullptr;


private:

    /**
     * @brief Build a description of the color attachment
     */
    void build_color_attachment(uint32_t imageIndex);

    /**
     * @brief Build a description of the rendering info
     */
    void build_rendering_info();

    /**
     * @brief Dynamically set the normally static pipeline parts within the command buffer
     */
    void set_dynamic_states();

    vk::RenderingInfoKHR renderingInfo = {};

    vk::RenderingAttachmentInfoKHR colorAttachment = {};

    Mesh* triangleMesh = nullptr;
};