#pragma once

#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
#include <vulkan/vulkan_raii.hpp>

#include <deque>
#include <functional>

/**
 * @brief Create a shader object
 * 
 * @param logicalDevice: Vulkan device
 * @param stage: Shader stadge for the module
 * @param filename: Name of the file holding the code
 * 
 * @return vk::ShaderExt: The created shader object
 */
std::vector<vk::raii::ShaderEXT> make_shader_objects
(
    vk::raii::Device& logicalDevice,
    const char* vertexFilename, const char* fragmentFilename
);