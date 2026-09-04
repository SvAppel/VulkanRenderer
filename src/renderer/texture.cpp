#include "texture.h"
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "image.h"
#include "descriptors.h"
#include "buffer.h"
#include "command.h"
#include "../logging/logger.h"

Texture::Texture(
    vk::raii::PhysicalDevice& physicalDevice,
    vk::raii::Device& logicalDevice,
    vk::raii::CommandPool& commandPool,
    vk::raii::Queue& queue,
    vk::raii::DescriptorSetLayout& descriptorSetLayout,
    vk::raii::DescriptorPool& descriptorPool,
    const char* filename):
    physicalDevice(physicalDevice), logicalDevice(logicalDevice), 
    commandPool(commandPool), queue(queue), descriptorSetLayout(descriptorSetLayout), descriptorPool(descriptorPool), filename(filename)
{
    load();

    image = make_image(logicalDevice, static_cast<uint32_t>(width), static_cast<uint32_t>(height), vk::Format::eR8G8B8A8Unorm, vk::ImageTiling::eOptimal, vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled, vk::MemoryPropertyFlagBits::eDeviceLocal);
    imageMemory = make_image_memory(physicalDevice, logicalDevice, vk::MemoryPropertyFlagBits::eDeviceLocal, image);
    
    populate();

    imageView = make_image_view(logicalDevice, image, vk::Format::eR8G8B8A8Unorm, vk::ImageAspectFlagBits::eColor);
    
    make_sampler();

    make_descriptor_set();
}

void Texture::use(vk::raii::CommandBuffer& commandBuffer, vk::raii::PipelineLayout& piplineLayout)
{
    commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, piplineLayout, 1, *descriptorSet, nullptr);
}

void Texture::load()
{
    Logger* logger = Logger::get_logger();
    std::stringstream lineBuilder;

    pixels = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);

    if(!pixels)
    {
        lineBuilder << "Failed to load texture: " << filename;
        logger->print_error(lineBuilder.str());
        return;
    }
}

void Texture::populate()
{
    vk::DeviceSize stagingBufferSize = width * height * STBI_rgb_alpha;
    auto[stagingBuffer, stagingMemory] = 
        create_buffer(physicalDevice, logicalDevice, stagingBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostCoherent | vk::MemoryPropertyFlagBits::eHostVisible);
    
    void* writeLocation = stagingMemory.mapMemory(0, stagingBufferSize);
    memcpy(writeLocation, pixels, stagingBufferSize);
    stagingMemory.unmapMemory();
    
    stbi_image_free(pixels);

    vk::raii::CommandBuffer commandBuffer = begin_single_time_commands(logicalDevice, commandPool);
    transition_image_layout
    (
        commandBuffer, image,
        vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal,
        vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eTransferWrite,
        vk::PipelineStageFlagBits2::eHost, vk::PipelineStageFlagBits2::eTransfer,
        vk::ImageAspectFlagBits::eColor
    );

    copy_buffer_to_image(commandBuffer, stagingBuffer, image, width, height);

    transition_image_layout
    (
        commandBuffer, image,
        vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal,
        vk::AccessFlagBits2::eTransferWrite, vk::AccessFlagBits2::eShaderRead,
        vk::PipelineStageFlagBits2::eTransfer, vk::PipelineStageFlagBits2::eFragmentShader,
        vk::ImageAspectFlagBits::eColor
    );
    end_single_time_commands(std::move(commandBuffer),queue);
}

void Texture::make_sampler()
{
    Logger* logger = Logger::get_logger();
    vk::PhysicalDeviceProperties properties = physicalDevice.getProperties();
    vk::SamplerCreateInfo samplerInfo
    {
        .magFilter = vk::Filter::eLinear,
        .minFilter = vk::Filter::eLinear,
        .mipmapMode = vk::SamplerMipmapMode::eLinear,
        .addressModeU = vk::SamplerAddressMode::eRepeat,
        .addressModeV = vk::SamplerAddressMode::eRepeat,
        .addressModeW = vk::SamplerAddressMode::eRepeat,
        .anisotropyEnable = true,
        .maxAnisotropy = properties.limits.maxSamplerAnisotropy,
        .compareEnable = false,
        .compareOp = vk::CompareOp::eAlways
    };
    samplerInfo.borderColor = vk::BorderColor::eIntOpaqueBlack;
    samplerInfo.unnormalizedCoordinates = vk::False;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 0.0f;

    sampler = vk::raii::Sampler(logicalDevice, samplerInfo);

    logger->print("Successfully created texture sampler.");
}

void Texture::make_descriptor_set()
{
    DescriptorManager descriptorManager = DescriptorManager(logicalDevice);
    descriptorSet = descriptorManager.allocate_descriptor_set(descriptorPool, descriptorSetLayout);

    vk::DescriptorImageInfo imageDescriptor 
    {
        .sampler = sampler,
        .imageView = imageView,
        .imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal
    };

    vk::WriteDescriptorSet descriptorWrite
    {
        .dstSet = descriptorSet,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eCombinedImageSampler,
        .pImageInfo = &imageDescriptor
    };

    logicalDevice.updateDescriptorSets(descriptorWrite, {});
}