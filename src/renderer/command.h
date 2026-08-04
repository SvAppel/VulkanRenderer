#pragma once

#include <vulkan/vulkan_raii.hpp>


vk::raii::CommandPool make_command_pool(vk::raii::Device& logicalDevice, uint32_t queueFamilyIndex);

vk::raii::CommandBuffer allocate_command_buffer(vk::raii::Device& logicalDevice, vk::raii::CommandPool& commandPool);

vk::raii::CommandBuffer begin_single_time_commands(vk::raii::Device& logicalDevice, vk::raii::CommandPool& commandPool);

void end_single_time_commands(vk::raii::CommandBuffer&& commandBuffer, vk::raii::Queue& queue);