#pragma once

#include <vulkan/vulkan_raii.hpp>
#include <vector>
#include <functional>
#include <glm/glm.hpp>


struct UniformBufferObject
{
    glm::mat4 model;
    glm::mat4 view;
    glm::mat4 proj;
};

class DescriptorManager
{
public:
    DescriptorManager(vk::raii::Device& logicalDevice);

    vk::raii::DescriptorSetLayout build_layout();

    void add_descriptor(vk::ShaderStageFlags stage, vk::DescriptorType type);

    vk::raii::DescriptorPool make_descriptor_pool(uint32_t descriptorSetCount, uint32_t bindingCount, vk::DescriptorType* pBindingTypes);

    vk::raii::DescriptorSet allocate_descriptor_set(vk::raii::DescriptorPool& descriptorPool, vk::raii::DescriptorSetLayout& descriptorSetLayout);

private:

    vk::raii::Device& logicalDevice;

    std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;

    void reset_layout();

};