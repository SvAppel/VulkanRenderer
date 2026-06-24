#include "synchronisation.h"

vk::raii::Semaphore make_semaphore(vk::raii::Device& logicalDevice)
{
    vk::SemaphoreCreateInfo semaphoreInfo{};

    vk::raii::Semaphore semaphore = vk::raii::Semaphore(logicalDevice, semaphoreInfo);

    return semaphore;
}

vk::raii::Fence make_fence(vk::raii::Device& logicalDevice)
{
    vk::FenceCreateInfo fenceInfo
    {
        .flags = vk::FenceCreateFlagBits::eSignaled
    };

    vk::raii::Fence fence = vk::raii::Fence(logicalDevice, fenceInfo);

    return fence;
}