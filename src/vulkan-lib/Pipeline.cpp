#include "Pipeline.hpp"
#include <vulkan-lib/Config.h>


#include "debug_lib/result.hpp"
#include "vulkan-lib/mesh.hpp"
#include "debug_lib/Logger.hpp"

namespace vkl {
    auto  make_pipeline_layout(vk::Device device, const std::vector<vk::DescriptorSetLayout>& descriptor_set_layouts) noexcept -> db::Result<vk::PipelineLayout> {
        vk::PipelineLayoutCreateInfo layoutInfo = {};
        layoutInfo.flags = vk::PipelineLayoutCreateFlags();
        layoutInfo.setLayoutCount = descriptor_set_layouts.size();
        layoutInfo.pSetLayouts = descriptor_set_layouts.data();

        layoutInfo.pushConstantRangeCount = 0;
        /*vk::PushConstantRange pushConstantInfo = {};
        pushConstantInfo.offset = 0;
        pushConstantInfo.size = sizeof(vkInit::ObjectData);
        pushConstantInfo.stageFlags = vk::ShaderStageFlagBits::eVertex;
        layoutInfo.pPushConstantRanges = &pushConstantInfo;*/
        vk::ResultValue<vk::PipelineLayout> layoutR =
            device.createPipelineLayout(layoutInfo);
        if (layoutR.result != vk::Result::eSuccess) {
            return db::error("Failed to creaet pipeline layout");
        }
        return layoutR.value;
    }
    auto make_render_pass(vk::Device device, vk::Format swapchainImageFormat) noexcept -> db::Result<vk::RenderPass> {
        //color
        vk::AttachmentDescription color_attachment = {};
        color_attachment.flags = vk::AttachmentDescriptionFlags();
        color_attachment.format = swapchainImageFormat;
        color_attachment.samples = vk::SampleCountFlagBits::e1;
        color_attachment.loadOp = vk::AttachmentLoadOp::eClear;
        color_attachment.storeOp = vk::AttachmentStoreOp::eStore;
        color_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        color_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        color_attachment.initialLayout = vk::ImageLayout::eUndefined;
        color_attachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

        vk::AttachmentReference color_attachment_ref = {};
        color_attachment_ref.attachment = 0;
        color_attachment_ref.layout = vk::ImageLayout::eColorAttachmentOptimal;

        //depth
        vk::AttachmentDescription depth_attachment{};
        depth_attachment.format = vk::Format::eD32Sfloat;
        depth_attachment.samples = vk::SampleCountFlagBits::e1;
        depth_attachment.loadOp = vk::AttachmentLoadOp::eClear;
        depth_attachment.storeOp = vk::AttachmentStoreOp::eDontCare;
        depth_attachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
        depth_attachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
        depth_attachment.initialLayout = vk::ImageLayout::eUndefined;
        depth_attachment.finalLayout = vk::ImageLayout::eDepthStencilAttachmentOptimal;

        vk::AttachmentReference depth_attachment_ref{};
        depth_attachment_ref.attachment = 1;
        depth_attachment_ref.layout = vk::ImageLayout::eDepthStencilAttachmentOptimal;


        std::array<vk::AttachmentDescription, 2> attachments = {
            color_attachment,   // index 0
            depth_attachment    // index 1
        };
        vk::SubpassDescription subpass = {};
        subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &color_attachment_ref;
        subpass.pDepthStencilAttachment = &depth_attachment_ref;

        vk::RenderPassCreateInfo renderpassInfo = {};
        renderpassInfo.flags = vk::RenderPassCreateFlags();
        renderpassInfo.attachmentCount = attachments.size();
        renderpassInfo.pAttachments = attachments.data();
        renderpassInfo.subpassCount = 1;
        renderpassInfo.pSubpasses = &subpass;

        vk::ResultValue<vk::RenderPass> renderpassR =
            device.createRenderPass(renderpassInfo);
        if (renderpassR.result != vk::Result::eSuccess)
            return db::error("Failed to create render pass");
        return renderpassR.value;
    }

    auto fillVertexInputStateCreateInfo() noexcept -> vk::PipelineVertexInputStateCreateInfo {
        vk::VertexInputBindingDescription bindingDescription =
            vkl::getBindingDescription(vkl::MeshType::eQuadTex);
        std::vector<vk::VertexInputAttributeDescription> attributeDescription =
            vkl::getAttributeDescription(vkl::MeshType::eQuadTex);

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescription.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
        return vertexInputInfo;
    }

    auto fillViewportScissor(GraphicsPipelineBundle& specifications) -> std::pair<vk::Viewport, vk::Rect2D> {
        vk::Viewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(specifications.extent.width);
        viewport.height = static_cast<float>(specifications.extent.height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vk::Rect2D scissor = {};
        scissor.offset.x = 0;
        scissor.offset.y = 0;
        scissor.extent = specifications.extent;
        return { viewport, scissor };
    }

    auto fillViewPortState(std::pair<vk::Viewport, vk::Rect2D>& viewportScissor) -> vk::PipelineViewportStateCreateInfo {
        vk::PipelineViewportStateCreateInfo viewPortState = {};
        viewPortState.flags = vk::PipelineViewportStateCreateFlags();
        viewPortState.viewportCount = 1;
        viewPortState.pViewports = &viewportScissor.first;
        viewPortState.scissorCount = 1;
        viewPortState.pScissors = &viewportScissor.second;
        return viewPortState;
    }


    auto fillColorBlendAttachment(vk::PipelineColorBlendAttachmentState* colorBlendAttachment) -> vk::PipelineColorBlendStateCreateInfo {
        vk::PipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.flags = vk::PipelineColorBlendStateCreateFlags();
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;
        return colorBlending;
    }
    auto fillRasterizer() -> vk::PipelineRasterizationStateCreateInfo {
        vk::PipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.flags = vk::PipelineRasterizationStateCreateFlags();
        rasterizer.depthBiasClamp = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eBack;
        rasterizer.frontFace = vk::FrontFace::eClockwise;
        rasterizer.depthBiasEnable = VK_FALSE;
        return rasterizer;
    }

    auto make_graphics_pipeline(GraphicsPipelineBundle& specifications) noexcept -> db::Result<GraphicsPipelineOutBundle> {
        // main pipeline
        vk::PipelineDepthStencilStateCreateInfo depth_stencil{};
        depth_stencil.depthTestEnable = VK_TRUE;
        depth_stencil.depthWriteEnable = VK_TRUE;
        depth_stencil.depthCompareOp = vk::CompareOp::eLess;
        depth_stencil.depthBoundsTestEnable = VK_FALSE;
        depth_stencil.stencilTestEnable = VK_FALSE;

        vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
        pipelineCreateInfo.pDepthStencilState = &depth_stencil;

        // a list of all the shader stages
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

        // VERTEX shader input
        vk::VertexInputBindingDescription bindingDescription =
            vkl::getBindingDescription(vkl::MeshType::eCubeTex);
        std::vector<vk::VertexInputAttributeDescription> attributeDescription =
            vkl::getAttributeDescription(vkl::MeshType::eCubeTex);

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
        vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
        vertexInputInfo.vertexBindingDescriptionCount = 1;
        vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
        vertexInputInfo.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescription.size());
        vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();

        // vk::PipelineVertexInputStateCreateInfo vertexInputInfo =
        // fillVertexInputStateCreateInfo();
        pipelineCreateInfo.pVertexInputState = &vertexInputInfo;

        vk::PipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
        inputAssemblyInfo.flags = vk::PipelineInputAssemblyStateCreateFlags();
        inputAssemblyInfo.topology = vk::PrimitiveTopology::eTriangleList;
        pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;

        // Vertex shader
        db::Logger::core_trace("Creating shader module");

        auto vertexShaderModRes = vkl::create_module(specifications.vertexFilepath,
            specifications.device);
        if (!vertexShaderModRes) {
            return db::error(std::format("Failed to create shader module for: {}", specifications.vertexFilepath), vertexShaderModRes.error());
        }
        vk::ShaderModule vertexShader = vertexShaderModRes.value();
        vk::PipelineShaderStageCreateInfo vertexShaderInfo = {};
        vertexShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
        vertexShaderInfo.stage = vk::ShaderStageFlagBits::eVertex;
        vertexShaderInfo.module = vertexShader;
        vertexShaderInfo.pName = "main";
        shaderStages.push_back(vertexShaderInfo);

        // viewport and scissor

        std::pair<vk::Viewport, vk::Rect2D> viewportScissor =
            fillViewportScissor(specifications);

        vk::PipelineViewportStateCreateInfo viewPortState =
            fillViewPortState(viewportScissor);
        pipelineCreateInfo.pViewportState = &viewPortState;

        // RASTERIZER
        vk::PipelineRasterizationStateCreateInfo rasterizer = fillRasterizer();
        pipelineCreateInfo.pRasterizationState = &rasterizer;

        // Fragment shader

        auto fragmentShaderRes = vkl::create_module(specifications.fragmentFilepath,
            specifications.device);
        if (!fragmentShaderRes) {
            return db::error(std::format("Failed to create shader module for: {}", specifications.fragmentFilepath), fragmentShaderRes.error());
        }
        vk::ShaderModule fragmentShader = fragmentShaderRes.value();

        vk::PipelineShaderStageCreateInfo fragmentShaderInfo = {};
        fragmentShaderInfo.flags = vk::PipelineShaderStageCreateFlags();
        fragmentShaderInfo.stage = vk::ShaderStageFlagBits::eFragment;
        fragmentShaderInfo.module = fragmentShader;
        fragmentShaderInfo.pName = "main";
        shaderStages.push_back(fragmentShaderInfo);

        pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
        pipelineCreateInfo.pStages = shaderStages.data();

        // Multisampling
        vk::PipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.flags = vk::PipelineMultisampleStateCreateFlags();
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = vk::SampleCountFlagBits::e1;
        pipelineCreateInfo.pMultisampleState = &multisampling;

        // color blend
        vk::PipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask =
            vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;
        vk::PipelineColorBlendStateCreateInfo colorBlending =
            fillColorBlendAttachment(&colorBlendAttachment);
        pipelineCreateInfo.pColorBlendState = &colorBlending;

        // Pipeline Layout
        auto layoutRes = make_pipeline_layout(specifications.device, specifications.descriptor_set_layouts);
        if (!layoutRes) {
            return db::error("Failed to make pipeline layout", layoutRes.error());
        }
        pipelineCreateInfo.layout = layoutRes.value();

        // renderpass
        auto renderpassRes = make_render_pass(specifications.device,
            specifications.swapchainImageFormat);
        if (!renderpassRes) {
            return db::error("Failed to make render pass", renderpassRes.error());
        }
        pipelineCreateInfo.renderPass = renderpassRes.value();

        // Extra stuff
        pipelineCreateInfo.basePipelineHandle = nullptr;
        vk::ResultValue<vk::Pipeline> pipelineR =
            specifications.device.createGraphicsPipeline(nullptr, pipelineCreateInfo);
        if (pipelineR.result != vk::Result::eSuccess) {
            return db::error("Failed to make graphics pipeline");
        }

        GraphicsPipelineOutBundle output = {};
        output.pipeline = pipelineR.value;
        output.renderpass = renderpassRes.value();
        output.layout = layoutRes.value();

        // after all is set up clean up the modules
        specifications.device.destroyShaderModule(vertexShader);
        specifications.device.destroyShaderModule(fragmentShader);
        return output;
    }
}
