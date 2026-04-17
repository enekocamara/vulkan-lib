module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.framebuffer;

import vulkan_lib.SwapchainFrame;
import debug_lib.result;

namespace vkl{
    export struct FramebufferInput{
        vk::Device device;
        vk::RenderPass renderPass;
        vk::Extent2D swapchainExtent;
    };
    
    export auto
        make_framebuffers(FramebufferInput inputBundle, std::vector<vkl::SwapchainFrame>& frames) -> db::Result<db::EmptyOk>;
}

