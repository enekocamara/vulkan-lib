#pragma once
#include "Config.h"
#include "QueueFamilies.hpp"

namespace vkl{
    struct GraphicsContext{
        vk::Device device;
        vk::PhysicalDevice physical_device;
        
        //family queue
        vkl::Queue graphics_queue;
        vkl::Queue present_queue;
        vkl::Queue transfer_queue;
        vkl::Queue compute_queue;

        vk::CommandPool graph_pres_command_pool;
        vk::CommandPool transfer_command_pool;
        vk::CommandPool compute_command_pool;

        vk::CommandBuffer graph_pres_command_buffer;
        vk::CommandBuffer transfer_command_buffer;
        vk::CommandBuffer compute_command_buffer;

        vk::DescriptorPool descriptor_pool;
    };

}