module;
#include "vulkan-lib/Config.h"
#include <functional>
#include <vector>
#include <cstdint>

export module vulkan_lib.commands;
import vulkan_lib.Swapchain;
import vulkan_lib.SwapchainFrame;
import debug_lib.result;


namespace vkl {
    
    export auto
        make_command_pool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint32_t queueFamilyIndex) -> db::Result<vk::CommandPool>;

    export auto
        make_command_buffer(vk::Device device, vk::CommandPool commandPool) -> db::Result<vk::CommandBuffer>;

    export auto
        make_frame_command_buffers(vk::Device device, vk::CommandPool commandPool, std::vector<SwapchainFrame>* frames) -> db::Result<db::EmptyOk>;

    export auto
        single_time_command(vk::Device device, vk::CommandPool command_pool, vk::Queue queue, std::function<db::Result<db::EmptyOk>(vk::CommandBuffer command_buffer)> lambda) -> db::Result<db::EmptyOk>;

    export auto
        begin_single_time_command(vk::Device device, vk::CommandPool command_pool) -> db::Result<vk::CommandBuffer>;

    export auto
        end_single_time_command(vk::Device device, vk::CommandPool command_pool, vk::CommandBuffer command_buffer, vk::Queue queue) -> db::Result<db::EmptyOk>;
}
