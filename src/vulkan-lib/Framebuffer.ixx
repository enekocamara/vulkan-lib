module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.framebuffer;

import vulkan_lib.swapchainFrame;
import <expected>;
import vulkan_lib.result;

namespace vkInit{
    export struct FramebufferInput{
        vk::Device device;
        vk::RenderPass renderPass;
        vk::Extent2D swapchainExtent;
    };
    
    export [[nodiscard]] inline auto 
    make_framebuffers(FramebufferInput inputBundle, std::vector<vkInit::SwapchainFrame>& frames) -> std::expected<EmptyOk, EmptyErr> {
        for (uint32_t i = 0; i < frames.size(); i++){
            std::vector<vk::ImageView> attachments = {
                frames[i].view
            };
            vk::FramebufferCreateInfo framebufferInfo = {};
            framebufferInfo.flags = vk::FramebufferCreateFlags();
            framebufferInfo.renderPass = inputBundle.renderPass;
            framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
            framebufferInfo.pAttachments = attachments.data();
            framebufferInfo.width = inputBundle.swapchainExtent.width;
            framebufferInfo.height = inputBundle.swapchainExtent.height;
            framebufferInfo.layers = 1;
            
            vk::ResultValue<vk::Framebuffer> framebufferR = inputBundle.device.createFramebuffer(framebufferInfo);
            if (framebufferR.result != vk::Result::eSuccess)
                return std::unexpected(EmptyErr{});
            frames[i].framebuffer = framebufferR.value;
        }
        return EmptyOk{};
    }

}

