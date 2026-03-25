module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.pipeline;

import <expected>;
import <iostream>;
import vulkan_lib.mesh;
import vulkan_lib.renderStructs;
import debug_lib.result;
import vulkan_lib.shader;
import debug_lib.Logger;

namespace vkl {

    export struct GraphicsPipelineBundle {
        vk::Device device;
        std::string vertexFilepath;
        std::string fragmentFilepath;
        vk::Extent2D extent;
        vk::Format swapchainImageFormat;
        std::vector<vk::DescriptorSetLayout> descriptor_set_layouts;
    };

    export struct GraphicsPipelineOutBundle {
        vk::PipelineLayout layout;
        vk::RenderPass renderpass;
        vk::Pipeline pipeline;
    };



    export [[nodiscard]] inline auto
        make_pipeline_layout(vk::Device device, const std::vector<vk::DescriptorSetLayout>& descriptor_set_layouts) noexcept -> db::Result<vk::PipelineLayout>;

    export [[nodiscard]] inline auto
        make_render_pass(vk::Device device, vk::Format swapchainImageFormat) noexcept -> db::Result<vk::RenderPass>; 

    export [[nodiscard]] inline auto
        fillVertexInputStateCreateInfo() noexcept -> vk::PipelineVertexInputStateCreateInfo;

    export [[nodiscard]] inline auto
        fillViewportScissor(GraphicsPipelineBundle& specifications) -> std::pair<vk::Viewport, vk::Rect2D>;

    export [[nodiscard]] inline auto
        fillViewPortState(std::pair<vk::Viewport, vk::Rect2D>& viewportScissor) -> vk::PipelineViewportStateCreateInfo;

    export [[nodiscard]] inline auto
        fillColorBlendAttachment(vk::PipelineColorBlendAttachmentState* colorBlendAttachment) -> vk::PipelineColorBlendStateCreateInfo;

    export [[nodiscard]] inline auto
        fillRasterizer() -> vk::PipelineRasterizationStateCreateInfo;

    export [[nodiscard]] inline auto
        make_graphics_pipeline(GraphicsPipelineBundle& specifications) noexcept -> db::Result<GraphicsPipelineOutBundle>;
} // namespace vkInit
