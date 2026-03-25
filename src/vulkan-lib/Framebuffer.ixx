module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.framebuffer;

import vulkan_lib.SwapchainFrame;
import debug_lib.result;
import <expected>;

namespace vkl{
    export struct FramebufferInput{
        vk::Device device;
        vk::RenderPass renderPass;
        vk::Extent2D swapchainExtent;
    };
    
    export [[nodiscard]] inline auto
        make_framebuffers(FramebufferInput inputBundle, std::vector<vkl::SwapchainFrame>& frames) -> db::Result<db::EmptyOk>;
}

