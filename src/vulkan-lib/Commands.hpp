#pragma once
#include "vulkan-lib/Config.h"
#include <functional>
#include <vector>
#include <cstdint>


#include "vulkan-lib/Swapchain.hpp"
#include "vulkan-lib/Swapchain.hpp"
#include "vulkan-lib/SwapchainFrame.hpp"
#include "debug_lib/Result.hpp"


namespace vkl {
    
    auto
        make_command_pool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint32_t queueFamilyIndex) -> db::Result<vk::CommandPool>;

    auto
        make_command_buffer(vk::Device device, vk::CommandPool commandPool) -> db::Result<vk::CommandBuffer>;

    auto
        make_frame_command_buffers(vk::Device device, vk::CommandPool commandPool, std::vector<SwapchainFrame>* frames) -> db::Result<db::EmptyOk>;

    auto
        single_time_command(vk::Device device, vk::CommandPool command_pool, vk::Queue queue, std::function<db::Result<db::EmptyOk>(vk::CommandBuffer command_buffer)> lambda) -> db::Result<db::EmptyOk>;

    auto
        begin_single_time_command(vk::Device device, vk::CommandPool command_pool) -> db::Result<vk::CommandBuffer>;

    auto
        end_single_time_command(vk::Device device, vk::CommandPool command_pool, vk::CommandBuffer command_buffer, vk::Queue queue) -> db::Result<db::EmptyOk>;
}
