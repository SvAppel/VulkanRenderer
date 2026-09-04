#include "image.h"

#include "buffer.h"
#include "../logging/logger.h"

vk::Format find_supported_format(vk::raii::PhysicalDevice& physicalDevice, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
    for (vk::Format format : candidates)
    {
        vk::FormatProperties properties = physicalDevice.getFormatProperties(format);

        if((tiling == vk::ImageTiling::eLinear) && (properties.linearTilingFeatures & features) == features)
            return format;

        if((tiling == vk::ImageTiling::eOptimal) && (properties.optimalTilingFeatures & features) == features)
            return format;
    }

    Logger* logger = Logger::get_logger();
    logger->print_error("Failed to find supported format!");

    return candidates[0];
}

vk::Format find_depth_format(vk::raii::PhysicalDevice& physicalDevice)
{
    return find_supported_format(physicalDevice, 
        {vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
        vk::ImageTiling::eOptimal, 
        vk::FormatFeatureFlagBits::eDepthStencilAttachment);
}

vk::raii::Image make_image
(
    vk::raii::Device& logicalDevice, 
    uint32_t width, uint32_t height,
    vk::Format format, vk::ImageTiling tiling, vk::ImageUsageFlags usage, 
    vk::MemoryPropertyFlags memoryProperties
)
{
    vk::ImageCreateInfo imageInfo
    {
        .imageType = vk::ImageType::e2D,
        .format = format,
        .extent = {width, height, 1},
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = vk::SampleCountFlagBits::e1,
        .tiling = tiling,
        .usage = usage,
        .sharingMode = vk::SharingMode::eExclusive
    };

    vk::raii::Image image = vk::raii::Image(logicalDevice, imageInfo);

    return std::move(image);
}

vk::raii::DeviceMemory make_image_memory
(
    vk::raii::PhysicalDevice& physicalDevice, 
    vk::raii::Device& logicalDevice,
    vk::MemoryPropertyFlags memoryProperties, 
    vk::raii::Image& image
)
{
    vk::MemoryRequirements memRequirements = image.getMemoryRequirements();
    vk::MemoryAllocateInfo allocInfo
    {
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = find_memory_type(physicalDevice, memRequirements.memoryTypeBits, memoryProperties)
    };

    vk::raii::DeviceMemory memory = vk::raii::DeviceMemory(logicalDevice, allocInfo);
    image.bindMemory(memory, 0);

    return std::move(memory);
}

vk::raii::ImageView make_image_view(vk::raii::Device& logicalDevice, vk::Image image, vk::Format format, vk::ImageAspectFlags aspect)
{
    vk::ImageViewCreateInfo createInfo
    {
        .image = image,
        .viewType = vk::ImageViewType::e2D,
        .format = format,
        .subresourceRange = 
        {
            .aspectMask = aspect,
            .levelCount = 1,
            .layerCount = 1
        }
    };

    vk::raii::ImageView imageView = vk::raii::ImageView(logicalDevice, createInfo);

    return imageView;
}

void copy_buffer_to_image(vk::raii::CommandBuffer& commandBuffer, const vk::raii::Buffer& buffer, vk::raii::Image& image, uint32_t width, uint32_t height)
{
    vk::BufferImageCopy region 
    {
        .bufferOffset = 0,
        .bufferRowLength = 0,
        .bufferImageHeight = 0,
        .imageSubresource = {.aspectMask = vk::ImageAspectFlagBits::eColor, .mipLevel = 0, .baseArrayLayer = 0, .layerCount = 1},
        .imageOffset = {0, 0, 0},
        .imageExtent = {width, height, 1}
    };
    commandBuffer.copyBufferToImage(buffer, image,vk::ImageLayout::eTransferDstOptimal, region);
}

void transition_image_layout(vk::raii::CommandBuffer& commandBuffer, vk::Image image,
    vk::ImageLayout oldLayout, vk::ImageLayout newLayout,
    vk::AccessFlags2 srcAccessMask, vk::AccessFlags2 dstAccessMask,
    vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage,
    vk::ImageAspectFlags aspect)
{
    vk::ImageSubresourceRange access
    {
        .aspectMask = aspect,
        .baseMipLevel = 0,
        .levelCount = 1,
        .baseArrayLayer = 0,
        .layerCount = 1
    };

    vk::ImageMemoryBarrier2 barrier
    {
        .srcStageMask = srcStage,
        .srcAccessMask = srcAccessMask,
        .dstStageMask = dstStage,
        .dstAccessMask = dstAccessMask,
        .oldLayout = oldLayout,
        .newLayout = newLayout,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = image,
        .subresourceRange = access
    };

    vk::DependencyInfo dependencyInfo
    {
        .dependencyFlags = {},
        .imageMemoryBarrierCount = 1,
        .pImageMemoryBarriers = &barrier
    };
    commandBuffer.pipelineBarrier2(dependencyInfo);
}