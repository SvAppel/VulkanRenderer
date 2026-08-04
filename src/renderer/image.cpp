#include "image.h"

#include "buffer.h"

vk::raii::Image make_image
(
    vk::raii::Device& logicalDevice, 
    uint32_t width, uint32_t height,
    vk::ImageTiling tiling, vk::ImageUsageFlags usage, 
    vk::MemoryPropertyFlags memoryProperties
)
{
    vk::ImageCreateInfo imageInfo
    {
        .imageType = vk::ImageType::e2D,
        .format = vk::Format::eR8G8B8A8Unorm,
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

vk::raii::ImageView make_image_view(vk::raii::Device& logicalDevice, vk::Image image, vk::Format format)
{
    vk::ImageViewCreateInfo createInfo
    {
        .image = image,
        .viewType = vk::ImageViewType::e2D,
        .format = format,
        .subresourceRange = 
        {
            .aspectMask = vk::ImageAspectFlagBits::eColor,
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
    vk::PipelineStageFlags2 srcStage, vk::PipelineStageFlags2 dstStage)
{
    vk::ImageSubresourceRange access
    {
        .aspectMask = vk::ImageAspectFlagBits::eColor,
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