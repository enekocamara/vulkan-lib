module;

#include "vulkan-lib/Config.h"

module vulkan_lib.framebuffer;

import vulkan_lib.SwapchainFrame;
import debug_lib.result;
import <expected>;
import <vector>;

namespace vkl {
    auto make_framebuffers(FramebufferInput inputBundle, std::vector<SwapchainFrame>& frames) -> db::Result<db::EmptyOk> {
        for (uint32_t i = 0; i < frames.size(); i++) {
            std::vector<vk::ImageView> attachments = {
                frames[i].m_view
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
                return db::error("Failed to create frame buffer");
            frames[i].m_framebuffer = framebufferR.value;
        }
        return db::EmptyOk{};
    }
}
