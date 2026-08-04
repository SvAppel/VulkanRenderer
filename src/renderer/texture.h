#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <stb_image.h>

class Texture 
{
public:

    Texture
    (
        vk::raii::PhysicalDevice& physicalDevice,
        vk::raii::Device& logicalDevice,
        vk::raii::CommandPool& commandPool,
        vk::raii::Queue& queue,
        vk::raii::DescriptorSetLayout& descriptorSetLayout,
        vk::raii::DescriptorPool& descriptorPool,
        const char* filename
    );

    void use(vk::raii::CommandBuffer& commandBuffer, vk::raii::PipelineLayout& piplineLayout);

    void load();

    void populate();

    void make_sampler();

    void make_descriptor_set();

    int width, height, channels;
    const char* filename;
    stbi_uc* pixels;

    vk::raii::Device& logicalDevice;
    vk::raii::PhysicalDevice& physicalDevice;

    //Ressources
    vk::raii::Image image = nullptr;
    vk::raii::DeviceMemory imageMemory = nullptr;
    vk::raii::ImageView imageView = nullptr;
    vk::raii::Sampler sampler = nullptr;

    //Ressource Descriptors
    vk::raii::DescriptorSet descriptorSet = nullptr;
    vk::raii::DescriptorSetLayout& descriptorSetLayout;
    vk::raii::DescriptorPool& descriptorPool;
    
    vk::raii::CommandPool& commandPool;
    vk::raii::Queue& queue;

};