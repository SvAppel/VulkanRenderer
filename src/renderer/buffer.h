#include <vulkan/vulkan_raii.hpp>
#include <utility>

std::pair<vk::raii::Buffer, vk::raii::DeviceMemory> create_buffer
(
    vk::raii::PhysicalDevice& physicalDevice,
    vk::raii::Device& logicalDevice,
    vk::DeviceSize size,
    vk::BufferUsageFlags usage,
    vk::MemoryPropertyFlags properties
);

void copy_buffer(
    vk::raii::Device& logicalDevice,
    vk::raii::Buffer& srcBuffer, 
    vk::raii::Buffer& dstBuffer, 
    vk::DeviceSize size, 
    vk::raii::CommandPool& commandPool, 
    vk::raii::Queue& queue
);

uint32_t find_memory_type(vk::raii::PhysicalDevice& physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);