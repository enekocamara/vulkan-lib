module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.pipeline;


import vulkan_lib.mesh;
import vulkan_lib.renderStructs;
import debug_lib.result;
import vulkan_lib.shader;

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



    export auto
        make_pipeline_layout(vk::Device device, const std::vector<vk::DescriptorSetLayout>& descriptor_set_layouts) noexcept -> db::Result<vk::PipelineLayout>;

    export [[nodiscard]] inline auto
        make_render_pass(vk::Device device, vk::Format swapchainImageFormat) noexcept -> db::Result<vk::RenderPass>; 

    export auto
        fillVertexInputStateCreateInfo() noexcept -> vk::PipelineVertexInputStateCreateInfo;

    export [[nodiscard]] auto
        fillViewportScissor(GraphicsPipelineBundle& specifications) -> std::pair<vk::Viewport, vk::Rect2D>;

    export [[nodiscard]] auto
        fillViewPortState(std::pair<vk::Viewport, vk::Rect2D>& viewportScissor) -> vk::PipelineViewportStateCreateInfo;

    export [[nodiscard]] auto
        fillColorBlendAttachment(vk::PipelineColorBlendAttachmentState* colorBlendAttachment) -> vk::PipelineColorBlendStateCreateInfo;

    export [[nodiscard]] auto
        fillRasterizer() -> vk::PipelineRasterizationStateCreateInfo;

    export auto
        make_graphics_pipeline(GraphicsPipelineBundle& specifications) noexcept -> db::Result<GraphicsPipelineOutBundle>;
} // namespace vkInit
