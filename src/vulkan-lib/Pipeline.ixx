module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.pipeline;

import <expected>;
import <iostream>;
import vulkan_lib.mesh;
import vulkan_lib.renderStructs;
import vulkan_lib.result;
import vulkan_lib.shader;
import vulkan_lib.logging;

namespace vkInit {

    export struct GraphicsPipelineBundle {
        vk::Device device;
        std::string vertexFilepath;
        std::string fragmentFilepath;
        vk::Extent2D extent;
        vk::Format swapchainImageFormat;
        vk::DescriptorSetLayout descriptorSetLayout;
    };

    export struct GraphicsPipelineOutBundle {
        vk::PipelineLayout layout;
        vk::RenderPass renderpass;
        vk::Pipeline pipeline;
    };



export [[nodiscard]] inline auto 
make_pipeline_layout(vk::Device device, vk::DescriptorSetLayout descriptorSetLayout) noexcept -> std::expected<vk::PipelineLayout, EmptyErr> {
    vk::PipelineLayoutCreateInfo layoutInfo = {};
    layoutInfo.flags = vk::PipelineLayoutCreateFlags();
    layoutInfo.setLayoutCount = 1;
    layoutInfo.pSetLayouts = &descriptorSetLayout;
    
    layoutInfo.pushConstantRangeCount = 0;
    /*vk::PushConstantRange pushConstantInfo = {};
    pushConstantInfo.offset = 0;
    pushConstantInfo.size = sizeof(vkInit::ObjectData);
    pushConstantInfo.stageFlags = vk::ShaderStageFlagBits::eVertex;
    layoutInfo.pPushConstantRanges = &pushConstantInfo;*/
    vk::ResultValue<vk::PipelineLayout> layoutR =
        device.createPipelineLayout(layoutInfo);
    if (layoutR.result != vk::Result::eSuccess) {
        if constexpr (_DEBUG)
            std::cerr << "failed to create pipeline layout\n";
        return std::unexpected(EmptyErr{});
    }
    return layoutR.value;
}

export [[nodiscard]] inline auto
make_render_pass(vk::Device device, vk::Format swapchainImageFormat) noexcept -> std::expected<vk::RenderPass, EmptyErr> {
  vk::AttachmentDescription colorAttachment = {};
  colorAttachment.flags = vk::AttachmentDescriptionFlags();
  colorAttachment.format = swapchainImageFormat;
  colorAttachment.samples = vk::SampleCountFlagBits::e1;
  colorAttachment.loadOp = vk::AttachmentLoadOp::eClear;
  colorAttachment.storeOp = vk::AttachmentStoreOp::eStore;
  colorAttachment.stencilLoadOp = vk::AttachmentLoadOp::eDontCare;
  colorAttachment.stencilStoreOp = vk::AttachmentStoreOp::eDontCare;
  colorAttachment.initialLayout = vk::ImageLayout::eUndefined;
  colorAttachment.finalLayout = vk::ImageLayout::ePresentSrcKHR;

  vk::AttachmentReference colorAttachmentRef = {};
  colorAttachmentRef.attachment = 0;
  colorAttachmentRef.layout = vk::ImageLayout::eColorAttachmentOptimal;

  vk::SubpassDescription subpass = {};
  subpass.flags = vk::SubpassDescriptionFlags();
  subpass.pipelineBindPoint = vk::PipelineBindPoint::eGraphics;
  subpass.colorAttachmentCount = 1;
  subpass.pColorAttachments = &colorAttachmentRef;

  vk::RenderPassCreateInfo renderpassInfo = {};
  renderpassInfo.flags = vk::RenderPassCreateFlags();
  renderpassInfo.attachmentCount = 1;
  renderpassInfo.pAttachments = &colorAttachment;
  renderpassInfo.subpassCount = 1;
  renderpassInfo.pSubpasses = &subpass;

  vk::ResultValue<vk::RenderPass> renderpassR =
      device.createRenderPass(renderpassInfo);
  if (renderpassR.result != vk::Result::eSuccess) {
    if constexpr (_DEBUG)
      std::cerr << "failed to make render pass\n";
    return std::unexpected(EmptyErr{});
  }
  return renderpassR.value;
}
export [[nodiscard]] inline auto
fillVertexInputStateCreateInfo() noexcept -> vk::PipelineVertexInputStateCreateInfo {
  vk::VertexInputBindingDescription bindingDescription =
      vkMesh::getPosColorBindingDescription();
  std::vector<vk::VertexInputAttributeDescription> attributeDescription =
      vkMesh::getPosColorAttributeDescriptions();

  vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
  vertexInputInfo.flags = vk::PipelineVertexInputStateCreateFlags();
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
  vertexInputInfo.vertexAttributeDescriptionCount =
      static_cast<uint32_t>(attributeDescription.size());
  vertexInputInfo.pVertexAttributeDescriptions = attributeDescription.data();
  return vertexInputInfo;
}

export [[nodiscard]] inline auto
fillViewportScissor(GraphicsPipelineBundle &specifications) -> std::pair<vk::Viewport, vk::Rect2D> {
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
  return {viewport, scissor};
}

export [[nodiscard]] inline auto
fillViewPortState(std::pair<vk::Viewport, vk::Rect2D> &viewportScissor) -> vk::PipelineViewportStateCreateInfo {
  vk::PipelineViewportStateCreateInfo viewPortState = {};
  viewPortState.flags = vk::PipelineViewportStateCreateFlags();
  viewPortState.viewportCount = 1;
  viewPortState.pViewports = &viewportScissor.first;
  viewPortState.scissorCount = 1;
  viewPortState.pScissors = &viewportScissor.second;
  return viewPortState;
}

export [[nodiscard]] inline auto 
fillColorBlendAttachment(vk::PipelineColorBlendAttachmentState *colorBlendAttachment) -> vk::PipelineColorBlendStateCreateInfo {
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

export [[nodiscard]] inline auto 
fillRasterizer() -> vk::PipelineRasterizationStateCreateInfo {
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

export [[nodiscard]] inline auto
make_graphics_pipeline(GraphicsPipelineBundle &specifications) noexcept -> std::expected<GraphicsPipelineOutBundle, EmptyErr> {
  // main pipeline
  vk::GraphicsPipelineCreateInfo pipelineCreateInfo = {};
  pipelineCreateInfo.flags = vk::PipelineCreateFlags();

  // a list of all the shader stages
  std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;

  // VERTEX shader input
  vk::VertexInputBindingDescription bindingDescription =
      vkMesh::getPosColorBindingDescription();
  std::vector<vk::VertexInputAttributeDescription> attributeDescription =
      vkMesh::getPosColorAttributeDescriptions();

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
  printDebug("creating vertex shader module...");

  
  auto vertexShaderModRes = vkInit::create_module(specifications.vertexFilepath,
                            specifications.device);
  if (!vertexShaderModRes) {
      return std::unexpected(EmptyErr{});
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
  printDebug("Creating fragment shader module...");

  auto fragmentShaderRes = vkInit::create_module(specifications.fragmentFilepath,
      specifications.device);
  if (!fragmentShaderRes) {
      return std::unexpected(EmptyErr{});
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
  printDebug("making pipeline layout...");
  auto layoutRes = make_pipeline_layout(specifications.device, specifications.descriptorSetLayout);
  if (!layoutRes) {
      return std::unexpected(EmptyErr{});
  }
  pipelineCreateInfo.layout = layoutRes.value();

  // renderpass
  printDebug("making renderpass ...");
  auto renderpassRes = make_render_pass(specifications.device,
                       specifications.swapchainImageFormat);
  if (!renderpassRes) {
      return  std::unexpected(EmptyErr{});
  }
  pipelineCreateInfo.renderPass = renderpassRes.value();

  // Extra stuff
  pipelineCreateInfo.basePipelineHandle = nullptr;
  printDebug("making pipeline ...");
  vk::ResultValue<vk::Pipeline> pipelineR =
      specifications.device.createGraphicsPipeline(nullptr, pipelineCreateInfo);
  if (pipelineR.result != vk::Result::eSuccess) {
    errprintDebug("failed to create graphics pipeline");
    return std::unexpected(EmptyErr{});
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
} // namespace vkInit
