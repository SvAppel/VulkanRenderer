#include "descriptors.h"

#include "../logging/logger.h"

    DescriptorManager::DescriptorManager(vk::raii::Device& logicalDevice): logicalDevice(logicalDevice) {}

    vk::raii::DescriptorSetLayout DescriptorManager::build_layout()
    {
        Logger* logger = Logger::get_logger();

        vk::DescriptorSetLayoutCreateInfo layoutInfo 
        {
            .bindingCount = static_cast<uint32_t>(layoutBindings.size()),
            .pBindings = layoutBindings.data()
        };

        vk::raii::DescriptorSetLayout layout = vk::raii::DescriptorSetLayout(logicalDevice, layoutInfo);

        logger->print("Successfully created descriptor set layout");
        reset_layout();

        return layout;
    }

    void DescriptorManager::add_descriptor(vk::ShaderStageFlags stage, vk::DescriptorType type)
    {
        vk::DescriptorSetLayoutBinding entry
        {
            .binding = static_cast<uint32_t>(layoutBindings.size()),
            .descriptorType = type,
            .descriptorCount = 1,
            .stageFlags = stage
        };

        layoutBindings.push_back(entry);
    }

    void DescriptorManager::reset_layout()
    {
        layoutBindings.clear();
    }

    vk::raii::DescriptorPool DescriptorManager::make_descriptor_pool(uint32_t descriptorSetCount, uint32_t bindingCount, vk::DescriptorType* pBindingTypes)
    {
        Logger* logger = Logger::get_logger();

        std::vector<vk::DescriptorPoolSize> poolSizes;
        for (int i = 0; i< bindingCount; i++)
        {
            vk::DescriptorPoolSize poolSize
            {
                .type = pBindingTypes[i],
                .descriptorCount = descriptorSetCount
            };
            poolSizes.push_back(poolSize);
        }

        vk::DescriptorPoolCreateInfo poolInfo 
        {
            .flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet,
            .maxSets = descriptorSetCount,
            .poolSizeCount = bindingCount,
            .pPoolSizes = poolSizes.data()
        };

        vk::raii::DescriptorPool pool = vk::raii::DescriptorPool(logicalDevice, poolInfo);

        logger->print("Successfully created Descriptor Pool");

        return pool;
    }

    vk::raii::DescriptorSet DescriptorManager::allocate_descriptor_set(vk::raii::DescriptorPool& descriptorPool, vk::raii::DescriptorSetLayout& descriptorSetLayout)
    {
        Logger* logger = Logger::get_logger();

        vk::DescriptorSetAllocateInfo allocateInfo 
        {
            .descriptorPool = descriptorPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &*descriptorSetLayout
        };

        std::vector<vk::raii::DescriptorSet> descriptorSets = logicalDevice.allocateDescriptorSets(allocateInfo);

        logger->print("Successfully created Descriptor Sets");

        return std::move(descriptorSets[0]);
    }