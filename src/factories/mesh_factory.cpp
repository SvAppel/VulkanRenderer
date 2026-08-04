#include "mesh_factory.h"

#include "../logging/logger.h"
#include "../renderer/buffer.h"
#include <iostream>
#include <utility>

// Mesh::Mesh(vk::raii::Buffer buffer, vk::DeviceSize offset): offset(offset)
// {
//     this->buffer = std::move(buffer);
// }

Mesh build_mesh(vk::raii::PhysicalDevice& physicalDevice, vk::raii::Device& logicalDevice, vk::raii::CommandPool& commandPool, vk::raii::Queue& queue)
{
    const std::vector<Vertex> vertices = {
        {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}, {1.0f, 0.0f}},
        {{0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}, {0.0f, 0.0f}},
        {{0.5f, 0.5f}, {0.0f, 0.0f, 1.0f}, {0.0f, 1.0f}},
        {{-0.5f, 0.5f}, {1.0f, 1.0f, 1.0f}, {1.0f, 1.0f}}
    };

    const std::vector<uint32_t> indices = {
        0, 1, 2, 2, 3, 0
    };

    Mesh mesh;
    mesh.vertexCount = vertices.size();
    mesh.indexCount = indices.size();

    //Create Vertex Buffer
    //Create the staging buffer visible to the CPU and GPU
    vk::DeviceSize vertexBufferSize = sizeof(vertices[0]) * vertices.size();
    auto [vertexStagingBuffer, vertexStagingMemory] = 
        create_buffer(physicalDevice, logicalDevice, vertexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void *vertexDataStaging = vertexStagingMemory.mapMemory(0, vertexBufferSize);
    memcpy(vertexDataStaging, vertices.data(), vertexBufferSize);
    vertexStagingMemory.unmapMemory();

    //Create the vertex buffer only visible to the GPU but much more optimized for rendering

    std::tie(mesh.vertexBuffer, mesh.vertexBufferMemory) = 
        create_buffer(physicalDevice, logicalDevice, vertexBufferSize, vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);
    
    copy_buffer(logicalDevice, vertexStagingBuffer, mesh.vertexBuffer, vertexBufferSize, commandPool, queue);

    //Create Index Buffer
    vk::DeviceSize indexBufferSize = sizeof(indices[0]) * indices.size();
    auto [indexStagingBuffer, indexStagingBufferMemory] = 
        create_buffer(physicalDevice, logicalDevice, indexBufferSize, vk::BufferUsageFlagBits::eTransferSrc, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

    void* indexDataStaging = indexStagingBufferMemory.mapMemory(0, indexBufferSize);
    memcpy(indexDataStaging, indices.data(), indexBufferSize);
    indexStagingBufferMemory.unmapMemory();

    std::tie(mesh.indexBuffer, mesh.indexBufferMemory) = 
        create_buffer(physicalDevice, logicalDevice, indexBufferSize, vk::BufferUsageFlagBits::eIndexBuffer | vk::BufferUsageFlagBits::eTransferDst, vk::MemoryPropertyFlagBits::eDeviceLocal);

    copy_buffer(logicalDevice, indexStagingBuffer, mesh.indexBuffer, indexBufferSize, commandPool, queue);

    return mesh;
}