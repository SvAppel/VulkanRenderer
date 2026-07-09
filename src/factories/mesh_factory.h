#pragma once

#include <glm/glm.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <vector>

struct Vertex
{
    glm::vec2 pos;
    glm::vec3 color;

    static vk::VertexInputBindingDescription2EXT getBindingDescription()
    {
        vk::VertexInputBindingDescription2EXT description
        {
            .binding = 0,
            .stride = sizeof(Vertex),
            .inputRate = vk::VertexInputRate::eVertex,
            .divisor = 1
        };

        return description;
    };

    static std::vector<vk::VertexInputAttributeDescription2EXT> getAttributeDescriptions()
    {
        std::vector<vk::VertexInputAttributeDescription2EXT> attributes(2);
        attributes[0].location = 0;
        attributes[0].binding = 0;
        attributes[0].format = vk::Format::eR32G32Sfloat;
        attributes[0].offset = offsetof(Vertex, pos);

        attributes[1].location = 1;
        attributes[1].binding = 0;
        attributes[1].format = vk::Format::eR32G32B32Sfloat;
        attributes[1].offset = offsetof(Vertex, color);

        return attributes;
    }
    
};

struct Mesh 
{
    // Mesh(vk::raii::Buffer buffer, vk::DeviceSize offset);
    //Mesh();

    vk::raii::Buffer vertexBuffer = nullptr;
    vk::raii::DeviceMemory vertexBufferMemory = nullptr;
    vk::DeviceSize vertexOffset = 0;
    uint32_t vertexCount = 0;
    vk::raii::Buffer indexBuffer = nullptr;
    vk::raii::DeviceMemory indexBufferMemory = nullptr;
    vk::DeviceSize indexOffset = 0;
    uint32_t indexCount = 0;
};

Mesh build_mesh(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& logicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue);

uint32_t find_memory_type(vk::raii::PhysicalDevice& physicalDevice, uint32_t typeFilter, vk::MemoryPropertyFlags properties);

