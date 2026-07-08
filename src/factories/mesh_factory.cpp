#include "mesh_factory.h"

#include "../logging/logger.h"
#include "../renderer/buffer.h"
#include <iostream>
#include <utility>

// Mesh::Mesh(vk::raii::Buffer buffer, vk::DeviceSize offset): offset(offset)
// {
//     this->buffer = std::move(buffer);
// }

Mesh build_triangle(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& logicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue)
{
    const std::vector<Vertex> vertices = 
    {
        {{-0.75f, 0.75f}, {1.0f, 0.0f, 0.0f}},
        {{0.75f, 0.75f}, {0.0f, 1.0f, 0.0f}},
        {{0.0f, -0.75f}, {0.0f, 0.0f, 1.0f}}
    };

    //Create the staging buffer visible to the CPU and GPU
    vk::DeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();
    auto [stagingBuffer, stagingMemory] = 
        create_buffer(physicalDevice, logicalDevice, bufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void *dataStaging = stagingMemory.mapMemory(0, bufferSize);
    memcpy(dataStaging, vertices.data(), bufferSize);
    stagingMemory.unmapMemory();

    //Create the vertex buffer only visible to the GPU but much more optimized for rendering
    Mesh mesh;
    std::tie(mesh.buffer, mesh.memory) = 
        create_buffer(physicalDevice, logicalDevice, bufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
    
    copy_buffer(logicalDevice, stagingBuffer, mesh.buffer, bufferSize, commandPool, queue);

    return mesh;
}