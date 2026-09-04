#pragma once

#include <vulkan/vulkan_raii.hpp>

#include "image.h"
#include "swapchain.h"
#include "texture.h"
#include "../factories/mesh_factory.h"

struct DepthAttachment
{
    vk::raii::Image image = nullptr;
    vk::raii::DeviceMemory memory = nullptr;
    vk::raii::ImageView imageView = nullptr;
    vk::Format format{};
};

/**
 * @brief Holds all the state used in one rendering/presentation operation.
 */
class Frame
{
public:
    Frame
    (
        Swapchain& swapchain, 
        vk::raii::PhysicalDevice& physicalDevice,
        vk::raii::Device& logicalDevice, 
        std::vector<vk::raii::ShaderEXT>& shaders, 
        vk::raii::CommandBuffer& commandBuffer,
        vk::raii::DescriptorSetLayout& descriptorSetLayout,
        vk::raii::DescriptorPool& descriptorPool,
        vk::raii::PipelineLayout& pipelineLayout,
        DepthAttachment& depthAttachment,
        Mesh* mesh,
        Texture* material
    );

    /**
     * @brief Set (and record) the command buffer
     * 
     * @param imageIndex: The index of the current image
     */
    void record_command_buffer(uint32_t imageIndex);

    void update_uniform_buffer();

    /**
     * @brief For recording drawing commands
     */
    vk::raii::CommandBuffer commandBuffer = nullptr;


    Swapchain& swapchain;

    std::vector<vk::ShaderEXT> rawShaders;



    vk::raii::Buffer uniformBuffer = nullptr;
    vk::raii::DeviceMemory uniformBufferMemory = nullptr;
    void* uniformBufferMapped = nullptr;

    vk::raii::DescriptorSetLayout& descriptorSetLayout;
    
    vk::raii::DescriptorPool& descriptorPool;
    
    vk::raii::PipelineLayout& pipelineLayout;
    
    vk::raii::DescriptorSet descriptorSet = nullptr;

    DepthAttachment& depthAttachment;


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
     * @brief Build a description of the depth attachment
     */
    void build_depth_attachment();

    /**
     * @brief Build a description of the rendering info
     */
    void build_rendering_info();

    /**
     * @brief Dynamically set the normally static pipeline parts within the command buffer
     */
    void set_dynamic_states();

    vk::RenderingInfoKHR renderingInfo = {};

    vk::RenderingAttachmentInfoKHR colorAttachmentInfo = {};
    vk::RenderingAttachmentInfoKHR depthAttachmentInfo = {};

    Mesh* mesh = nullptr;
    Texture* material = nullptr;
};