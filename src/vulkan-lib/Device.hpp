#pragma once

#include "vulkan-lib/Config.h"

#include "vulkan-lib/QueueFamilies.hpp"
#include <debug_lib/Result.hpp>

namespace vkl{
       
    auto
        choose_physical_device(vk::Instance& instance) noexcept -> db::Result<vk::PhysicalDevice>;
   
    ///prints the family properties. does not check for _DEBUG
    auto
        print_queue_family_properties(const vk::QueueFamilyProperties& properties)noexcept -> void;

    auto
        create_device(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface) noexcept -> db::Result<vk::Device>;

    ///gets [graphics queue,present queue] from device, 
    auto
        get_queue(vk::PhysicalDevice physical_device, vk::Device device, vk::SurfaceKHR surface) noexcept -> vkl::QueueFamilyIndices;
}
