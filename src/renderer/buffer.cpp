#include "buffer.h"

#include "command.h"
#include "../logging/logger.h"

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> create_buffer
(
    vk::raii::PhysicalDevice& physicalDevice,
    vk::raii::Device& logicalDevice,
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties
)
{
    vk::BufferCreateInfo bufferInfo
    {
        .size = size,
        .usage = usage,
        .sharingMode = vk::SharingMode::eExclusive
    };
    vk::raii::Buffer buffer = vk::raii::Buffer(logicalDevice, bufferInfo);

    vk::MemoryRequirements memRequirements = buffer.getMemoryRequirements();
    vk::MemoryAllocateInfo allocationInfo
    {
        .allocationSize = memRequirements.size,
        .memoryTypeIndex = find_memory_type(physicalDevice, memRequirements.memoryTypeBits, properties)
    };
    vk::raii::DeviceMemory bufferMemory = vk::raii::DeviceMemory(logicalDevice, allocationInfo);
    buffer.bindMemory(*bufferMemory, 0);

    return {std::move(buffer), std::move(bufferMemory)};
}

uint32_t find_memory_type(vk::raii::PhysicalDevice& physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties)
{
    Logger* logger = Logger::get_logger();

    vk::PhysicalDeviceMemoryProperties memProperties = physicalDevice.getMemoryProperties();

    for(uint32_t i = 0; i < memProperties.memoryTypeCount; i++)
    {
        if((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties)
        {
            return i;
        }
    }

    logger->print_error("Failed to find suitable memory type!");
    return 0;
}

void copy_buffer(vk::raii::Device& logicalDevice, vk::raii::Buffer& srcBuffer, vk::raii::Buffer& dstBuffer, vk::DeviceSize size, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue)
{
    // vk::CommandBufferAllocateInfo allocationInfo
    // {
    //     .commandPool = commandPool,
    //     .level = vk::CommandBufferLevel::ePrimary,
    //     .commandBufferCount = 1
    // };
    // vk::raii::CommandBuffer commandCopyBuffer = std::move(logicalDevice.allocateCommandBuffers(allocationInfo).front());
    vk::raii::CommandBuffer commandCopyBuffer = begin_single_time_commands(logicalDevice, commandPool);
    //commandCopyBuffer.begin({.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit});
    commandCopyBuffer.copyBuffer(*srcBuffer, *dstBuffer, vk::BufferCopy(0, 0, size));
    // commandCopyBuffer.end();
    
    // queue.submit(vk::SubmitInfo
    //     {
    //         .commandBufferCount = 1,
    //         .pCommandBuffers = &*commandCopyBuffer
    //     }, nullptr);
    // queue.waitIdle();
    end_single_time_commands(std::move(commandCopyBuffer), queue);
}