#include "image.h"

vk::raii::ImageView create_image_view(vk::raii::Device& logicalDevice, vk::Image image, vk::Format format)
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