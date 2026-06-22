#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>


vk::raii::CommandPool make_command_pool(vk::raii::Device& devlogicalDeviceice, uint32_t queueFamilyindex);

vk::raii::CommandBuffer allocate_command_buffer(vk::raii::Device& logicalDevice, vk::raii::CommandPool& commandPool);