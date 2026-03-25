module;

#include "vulkan-lib/Config.h"
export module vulkan_lib.commands;

import vulkan_lib.Swapchain;
import vulkan_lib.SwapchainFrame;
import debug_lib.result;
import <expected>;
import <functional>;

namespace vkl {
    export struct CommandBufferInputBundle{
        vk::Device device;
        vk::CommandPool commandPool;
        std::vector<SwapchainFrame>& frames;
    };
    export [[nodiscard]] auto
        make_command_pool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint32_t queueFamilyIndex) -> db::Result<vk::CommandPool>;

    export [[nodiscard]] auto
        make_command_buffer(CommandBufferInputBundle inputBundle) -> db::Result<vk::CommandBuffer>;

    export [[nodiscard]] auto
        make_frame_command_buffers(CommandBufferInputBundle inputBundle) -> db::Result<db::EmptyOk>;

    export [[nodiscard]] auto
        single_time_command(vk::Device device, vk::CommandPool command_pool, std::function<std::expected<db::EmptyOk, db::Error>()> lambda) -> db::Result<db::EmptyOk>;

    export [[nodiscard]] auto
        begin_single_time_command(vk::Device device, vk::CommandPool command_pool) -> db::Result<vk::CommandBuffer>;

    export [[nodiscard]] auto
        end_single_time_command(vk::Device device, vk::CommandPool command_pool, vk::CommandBuffer command_buffer, vk::Queue queue) -> db::Result<db::EmptyOk>;
}
