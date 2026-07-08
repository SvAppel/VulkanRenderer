#pragma once

#include <vulkan/vulkan_raii.hpp>

vk::raii::Semaphore make_semaphore(vk::raii::Device& logicalDevice);

vk::raii::Fence make_fence(vk::raii::Device& logicalDevice);