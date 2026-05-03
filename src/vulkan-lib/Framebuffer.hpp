#pragma once
#include "vulkan-lib/Config.h"


#include "vulkan-lib/SwapchainFrame.hpp"
#include "debug_lib/result.hpp"

namespace vkl{
    struct FramebufferInput{
        vk::Device device;
        vk::RenderPass renderPass;
        vk::Extent2D swapchainExtent;
    };
    
    auto
        make_framebuffers(FramebufferInput inputBundle, std::vector<vkl::SwapchainFrame>& frames) -> db::Result<db::EmptyOk>;
}

