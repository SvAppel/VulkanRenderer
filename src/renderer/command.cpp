#include "command.h"

#include "../logging/logger.h"

vk::raii::CommandPool make_command_pool(vk::raii::Device& logicalDevice, uint32_t queueFamilyindex)
{
    Logger* logger = Logger::get_logger();

    vk::CommandPoolCreateInfo poolInfo
    {
        .flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer,
        .queueFamilyIndex = queueFamilyindex
    };

    vk::raii::CommandPool pool = vk::raii::CommandPool(logicalDevice, poolInfo);

    logger->print("Command pool created successfully");

    return pool;
}

vk::raii::CommandBuffer allocate_command_buffer(vk::raii::Device& logicalDevice, vk::raii::CommandPool& commandPool)
{
    vk::CommandBufferAllocateInfo allocInfo
    {
        .commandPool = commandPool,
        .level = vk::CommandBufferLevel::ePrimary,
        .commandBufferCount = 1
    };

    vk::raii::CommandBuffer commandBuffer = std::move(logicalDevice.allocateCommandBuffers(allocInfo).front());

    return commandBuffer;
}