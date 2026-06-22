#include "frame.h"

Frame::Frame(vk::Image image, vk::raii::Device& logicalDevice, vk::Format swapchainFormat): 
    image(image)
{
    imageView = create_image_view(logicalDevice, image, swapchainFormat);

}

void Frame::set_command_buffer(vk::raii::CommandBuffer& newCommandBuffer, std::vector<vk::raii::ShaderEXT>& shaders, vk::Extent2D frameSize)
{
    commandBuffer = std::move(newCommandBuffer);

    build_color_attachment();
    build_rendering_info(frameSize);

    vk::CommandBufferBeginInfo beginInfo{};

    commandBuffer.begin(beginInfo);
    
        transition_image_layout
        (
            commandBuffer, image,
            vk::ImageLayout::eUndefined, vk::ImageLayout::eAttachmentOptimal,
            vk::AccessFlagBits2::eNone, vk::AccessFlagBits2::eColorAttachmentWrite,
            vk::PipelineStageFlagBits2::eTopOfPipe, vk::PipelineStageFlagBits2::eFragmentShader
        );

        //Instead of creating a static pipeline we setup the pipeline dynamically via command buffer instructions
        set_dynamic_states(frameSize);

        commandBuffer.beginRenderingKHR(renderingInfo);

            vk::ShaderStageFlagBits stages[2] = {vk::ShaderStageFlagBits::eVertex, vk::ShaderStageFlagBits::eFragment};
            //The RAII shaders cannot be passed into the command buffer as is. They need to be converted to a vector of raw handles first
            //Command buffer recording methods take raw handles because they only record references, not ownership. 
            //The RAII wrappers manage lifetime separately.
            std::vector<vk::ShaderEXT> rawShaders(shaders.size());
            for (uint32_t i = 0; i < shaders.size(); i++)
            rawShaders[i] = shaders[i];
            commandBuffer.bindShadersEXT(stages, rawShaders);

            commandBuffer.draw(3, 1, 0, 0);

        commandBuffer.endRenderingKHR();

        transition_image_layout
        (
            commandBuffer, image,
            vk::ImageLayout::eAttachmentOptimal, vk::ImageLayout::ePresentSrcKHR,
            vk::AccessFlagBits2::eColorAttachmentWrite, vk::AccessFlagBits2::eNone,
            vk::PipelineStageFlagBits2::eFragmentShader, vk::PipelineStageFlagBits2::eBottomOfPipe
        );

    commandBuffer.end();
}

void Frame::build_rendering_info(vk::Extent2D frameSize)
{
    vk::RenderingInfoKHR tempRenderingInfo
    {
        .renderArea = vk::Rect2D({0,0}, frameSize),
        .layerCount = 1,
        //bitmask indicating the layers which will be rendered to
        .viewMask = 0,
        .colorAttachmentCount = 1,
        .pColorAttachments = &colorAttachment
    };

    renderingInfo = tempRenderingInfo;
}

void Frame::build_color_attachment()
{
    vk::RenderingAttachmentInfoKHR tempColorAttachment
    {
        .imageView = imageView,
        .imageLayout = vk::ImageLayout::eAttachmentOptimal,
        .loadOp = vk::AttachmentLoadOp::eClear,
        .storeOp = vk::AttachmentStoreOp::eStore,
        .clearValue = vk::ClearValue({0.5f, 0.0f, 0.25f, 1.0f})
    };

    colorAttachment = tempColorAttachment;
}

void Frame::set_dynamic_states(vk::Extent2D frameSize)
{
    vk::Viewport viewport
    {
        .x = 0.0f, .y = 0.0f,
        .width = frameSize.width, .height = frameSize.height,
        .minDepth = 0.0f, .maxDepth = 1.0f

    };
    commandBuffer.setViewportWithCount(viewport);

    vk::Rect2D scissor = vk::Rect2D({0,0}, frameSize);
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