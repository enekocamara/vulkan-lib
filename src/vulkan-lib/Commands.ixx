module;

#include "vulkan-lib/Config.h"
export module vulkan_lib.commands;

import <expected>;
//#include "Config.h"
//#include "Queue_families.h"
import vulkan_lib.swapchain;
import vulkan_lib.swapchainFrame;
import vulkan_lib.result;

namespace vkInit {
    export struct CommandBufferInputBundle{
        vk::Device device;
        vk::CommandPool commandPool;
        std::vector<vkInit::SwapchainFrame>& frames;
    };
    export [[nodiscard]] inline std::expected<vk::CommandPool, EmptyErr> make_command_pool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint32_t queueFamilyIndex){ 
        vk::CommandPoolCreateInfo poolInfo = {};
        poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
        poolInfo.queueFamilyIndex = queueFamilyIndex;

        vk::ResultValue<vk::CommandPool> commandPoolR = device.createCommandPool(poolInfo);
        if (commandPoolR.result != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        return commandPoolR.value;
    }

    export [[nodiscard]] inline std::expected<vk::CommandBuffer, EmptyErr> make_command_buffer(CommandBufferInputBundle inputBundle) {
        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.commandPool = inputBundle.commandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;

        vk::ResultValue<std::vector<vk::CommandBuffer>> commandBufferR = inputBundle.device.allocateCommandBuffers(allocInfo);
        if (commandBufferR.result != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        return commandBufferR.value[0];
    }
    export [[nodiscard]] inline auto make_frame_command_buffers(CommandBufferInputBundle inputBundle) -> std::expected<EmptyOk, EmptyErr> {
        vk::CommandBufferAllocateInfo allocInfo = {};
        allocInfo.commandPool = inputBundle.commandPool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = 1;
        for (int i = 0; i < inputBundle.frames.size(); i++){
            vk::ResultValue<std::vector<vk::CommandBuffer>> commandBufferR = inputBundle.device.allocateCommandBuffers(allocInfo);
            if (commandBufferR.result != vk::Result::eSuccess)
                return std::unexpected(EmptyErr{});
            inputBundle.frames[i].commandbuffer = commandBufferR.value[0];
        }
        return EmptyOk{};
    }
}
