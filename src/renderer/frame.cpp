#include "frame.h"
#include "synchronisation.h"
#include "descriptors.h"
#include "buffer.h"
#include "../factories/mesh_factory.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>


Frame::Frame(Swapchain& swapchain, 
    vk::raii::PhysicalDevice& physicalDevice,
    vk::raii::Device& logicalDevice, 
    std::vector<vk::raii::ShaderEXT>& shaders, 
    vk::raii::CommandBuffer& commandBuffer,
    vk::raii::DescriptorSetLayout& descriptorSetLayout,
    vk::raii::DescriptorPool& descriptorPool,
    vk::raii::PipelineLayout& pipelineLayout, 
    Mesh* mesh): 
    swapchain(swapchain), descriptorSetLayout(descriptorSetLayout), descriptorPool(descriptorPool), pipelineLayout(pipelineLayout)
{
    this->commandBuffer = std::move(commandBuffer);
    this->mesh = mesh;

    rawShaders.reserve(shaders.size());
    for (uint32_t i = 0; i < shaders.size(); i++)
        rawShaders.push_back(shaders[i]);

    imageAuqiredSemaphore = make_semaphore(logicalDevice);
	renderFinishedSemaphore = make_semaphore(logicalDevice);
	renderFinishedFence = make_fence(logicalDevice);

    //Create uniform buffer object
    vk::DeviceSize bufferSize = sizeof(UniformBufferObject);
    std::tie(uniformBuffer, uniformBufferMemory) = 
        create_buffer(physicalDevice, logicalDevice, bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
    uniformBufferMapped = uniformBufferMemory.mapMemory(0, bufferSize);

    DescriptorManager descriptorManager = DescriptorManager(logicalDevice);
    descriptorSet = descriptorManager.allocate_descriptor_set(descriptorPool, descriptorSetLayout);

    vk::DescriptorBufferInfo bufferInfo
    {
        .buffer = uniformBuffer,
        .offset = 0,
        .range = sizeof(UniformBufferObject)
    };

    vk::WriteDescriptorSet descriptorWrite
    {
        .dstSet = descriptorSet,
        .dstBinding = 0,
        .dstArrayElement = 0,
        .descriptorCount = 1,
        .descriptorType = vk::DescriptorType::eUniformBuffer,
        .pBufferInfo = &bufferInfo
    };

    logicalDevice.updateDescriptorSets(descriptorWrite, {});
}

void Frame::record_command_buffer(uint32_t imageIndex)
{
    commandBuffer.reset();

    build_color_attachment(imageIndex);
    build_rendering_info();

    vk::CommandBufferBeginInfo beginInfo{};

    commandBuffer.begin(beginInfo);
    
        transition_image_layout
        (
            commandBuffer, swapchain.images[imageIndex],
            vk::ImageLayout::eUndefined, vk::ImageLayout::eColorAttachmentOptimal,
            vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eColorAttachmentWrite,
            // INFO: This works but is inefficient. 
            // The destination pipeline stage only needs to be set to when the image needs to be accessed.
            // Otherwise the image sits unnecessarily long in its layout.
            // Since the image isnt used beforehand i.e. as a texture to be read from, 
            // the earliest necessary stage is eColorAttachmentOutput, when the pixel colors are actually written to the image
            //vk::PipelineStageFlagBits2::eTopOfPipe, vk::PipelineStageFlagBits2::eFragmentShader
            vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eColorAttachmentOutput
        );

        //Instead of creating a static pipeline we setup the pipeline dynamically via command buffer instructions
        set_dynamic_states();

        commandBuffer.beginRenderingKHR(renderingInfo);

            vk::ShaderStageFlagBits stages[2] = {vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment};

            
            commandBuffer.bindShadersEXT(stages, rawShaders);

            //Bind triangle mesh
            commandBuffer.bindVertexBuffers(0, *mesh->vertexBuffer, mesh->vertexOffset);
            commandBuffer.bindIndexBuffer(*mesh->indexBuffer, mesh->indexOffset, vk::IndexType::eUint32);
            commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, pipelineLayout, 0, *descriptorSet, nullptr);

            commandBuffer.drawIndexed(mesh->indexCount, 1, 0, mesh->indexOffset, 0);

        commandBuffer.endRenderingKHR();

        transition_image_layout
        (
            commandBuffer, swapchain.images[imageIndex],
            vk::ImageLayout::eColorAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite, vk::AccessFlagBits2::eNone,
            //vk::PipelineStageFlagBits2::eFragmentShader, vk::PipelineStageFlagBits2::eBottomOfPipe
            vk::PipelineStageFlagBits2::eColorAttachmentOutput, vk::PipelineStageFlagBits2::eBottomOfPipe
        );

    commandBuffer.end();
}

void Frame::update_uniform_buffer()
{
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(currentTime - startTime).count();

    UniformBufferObject ubo{};
    ubo.model = glm::rotate(glm::mat4(1.0f), time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.view = glm::lookAt(glm::vec3(2.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    ubo.proj = glm::perspective(glm::radians(45.0f), static_cast<float>(swapchain.extent.width) / static_cast<float>(swapchain.extent.height), 0.1f, 10.0f);

    // invert y coordinates to account for OpenGL standard of GLM
    ubo.proj[1][1] *=-1;

    memcpy(uniformBufferMapped, &ubo, sizeof(ubo));
}

void Frame::build_color_attachment(uint32_t imageIndex)
{
    vk::RenderingAttachmentInfoKHR tempColorAttachment
    {
        .imageView = swapchain.imageViews[imageIndex],
        .imageLayout = vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = vk::ClearValue({0.5f, 0.0f, 0.25f, 1.0f})
    };

    colorAttachment = tempColorAttachment;
}

void Frame::build_rendering_info()
{
    vk::RenderingInfoKHR tempRenderingInfo
    {
        .renderArea = vk::Rect2D({0,0}, swapchain.extent),
        .layerCount = 1,
        //bitmask indicating the layers which will be rendered to
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment
    };

    renderingInfo = tempRenderingInfo;
}

void Frame::set_dynamic_states()
{
    vk::VertexInputBindingDescription2EXT binding = Vertex::getBindingDescription();
    std::vector<vk::VertexInputAttributeDescription2EXT> attributes = Vertex::getAttributeDescriptions();

    commandBuffer.setVertexInputEXT(binding, attributes);


    vk::Viewport viewport
    {
        .x = 0.0f, .y = 0.0f,
        .width = (float)swapchain.extent.width, .height = (float)swapchain.extent.height,
        .minDepth = 0.0f, .maxDepth = 1.0f

    };
    commandBuffer.setViewportWithCount(viewport);

    vk::Rect2D scissor = vk::Rect2D({0,0}, swapchain.extent);
    commandBuffer.setScissorWithCount(scissor);

    commandBuffer.setRasterizerDiscardEnable(0);
    commandBuffer.setPolygonModeEXT(vk::PolygonMode::eFill);
    commandBuffer.setRasterizationSamplesEXT(vk::SampleCountFlagBits::e1);
    uint32_t sampleMask = 1;
    commandBuffer.setSampleMaskEXT(vk::SampleCountFlagBits::e1, sampleMask);
    commandBuffer.setAlphaToCoverageEnableEXT(0);
    commandBuffer.setCullMode(vk::CullModeFlagBits::eNone);

    commandBuffer.setDepthTestEnable(0);
    commandBuffer.setDepthWriteEnable(0);
    commandBuffer.setDepthBiasEnable(0);
    commandBuffer.setStencilTestEnable(0);

    commandBuffer.setPrimitiveTopology(vk::PrimitiveTopology::eTriangleList);
    commandBuffer.setPrimitiveRestartEnable(0);

    uint32_t colorBlendEnable = 0;
    commandBuffer.setColorBlendEnableEXT(0, colorBlendEnable);
    vk::ColorBlendEquationEXT equation
    {
        .srcColorBlendFactor = vk::BlendFactor::eOne,
        .dstColorBlendFactor = vk::BlendFactor::eZero,
        .colorBlendOp = vk::BlendOp::eAdd
    };
    commandBuffer.setColorBlendEquationEXT(0, equation);
    vk::ColorComponentFlags colorWriteMask = vk::ColorComponentFlagBits::eR
        | vk::ColorComponentFlagBits::eG
        | vk::ColorComponentFlagBits::eB
        | vk::ColorComponentFlagBits::eA;
    commandBuffer.setColorWriteMaskEXT(0, colorWriteMask);
}