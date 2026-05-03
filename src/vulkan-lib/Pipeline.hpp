#pragma once
#include "vulkan-lib/Config.h"



#include "vulkan-lib/mesh.hpp"
#include "vulkan-lib/RenderStructs.hpp"
#include "debug_lib/result.hpp"
#include "vulkan-lib/shader.hpp"

namespace vkl {

    struct GraphicsPipelineBundle {
        vk::Device device;
        std::string vertexFilepath;
        std::string fragmentFilepath;
        vk::Extent2D extent;
        vk::Format swapchainImageFormat;
        std::vector<vk::DescriptorSetLayout> descriptor_set_layouts;
    };

    struct GraphicsPipelineOutBundle {
        vk::PipelineLayout layout;
        vk::RenderPass renderpass;
        vk::Pipeline pipeline;
    };



    auto
        make_pipeline_layout(vk::Device device, const std::vector<vk::DescriptorSetLayout>& descriptor_set_layouts) noexcept -> db::Result<vk::PipelineLayout>;

    [[nodiscard]] inline auto
        make_render_pass(vk::Device device, vk::Format swapchainImageFormat) noexcept -> db::Result<vk::RenderPass>; 

    auto
        fillVertexInputStateCreateInfo() noexcept -> vk::PipelineVertexInputStateCreateInfo;

    [[nodiscard]] auto
        fillViewportScissor(GraphicsPipelineBundle& specifications) -> std::pair<vk::Viewport, vk::Rect2D>;

    [[nodiscard]] auto
        fillViewPortState(std::pair<vk::Viewport, vk::Rect2D>& viewportScissor) -> vk::PipelineViewportStateCreateInfo;

    [[nodiscard]] auto
        fillColorBlendAttachment(vk::PipelineColorBlendAttachmentState* colorBlendAttachment) -> vk::PipelineColorBlendStateCreateInfo;

    [[nodiscard]] auto
        fillRasterizer() -> vk::PipelineRasterizationStateCreateInfo;

    auto
        make_graphics_pipeline(GraphicsPipelineBundle& specifications) noexcept -> db::Result<GraphicsPipelineOutBundle>;
} // namespace vkInit
