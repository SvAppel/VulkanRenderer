#pragma once

#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_NO_STRUCT_CONSTRUCTORS
//#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <deque>
#include <functional>

/**
* @brief Create a Vulkan instance
*
* @param applicationName: The name of the application
* @param deletionQueue: Queue onto which to push the instance's destructor
*
* @return The instance created
*/
vk::Instance make_instance(const char* applicationName, vk::raii::Context* context, std::deque<std::function<void()>>& deletionQueue);
