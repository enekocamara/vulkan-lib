#include "Framebuffer.hpp"

#include "vulkan-lib/Config.h"
#include <expected>
#include <vector>

#include "vulkan-lib/SwapchainFrame.hpp"
#include "debug_lib/result.hpp"


namespace vkl {
    auto make_framebuffers(FramebufferInput inputBundle, std::vector<SwapchainFrame>& frames) -> db::Result<db::EmptyOk> {
        for (uint32_t i = 0; i < frames.size(); i++) {
            std::vector<vk::ImageView> attachments = {
                frames[i].m_view,
                frames[i].m_depth_view
            };
            vk::FramebufferCreateInfo framebufferInfo = {};
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
