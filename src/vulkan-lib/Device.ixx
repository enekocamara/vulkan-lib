module;

#include "vulkan-lib/Config.h"
export module vulkan_lib.device;

import <expected>;
import <iostream>;
import <format>;
import debug_lib.Logger;
import vulkan_lib.queue_families;
import debug_lib.result;

namespace vkl{
       
    export [[nodiscard]] auto
        choose_physical_device(vk::Instance& instance) noexcept -> db::Result<vk::PhysicalDevice>;
   
    ///prints the family properties. does not check for _DEBUG
    export auto
        print_queue_family_properties(const vk::QueueFamilyProperties& properties)noexcept -> void;

    export [[nodiscard]] auto
        create_device(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface) noexcept -> db::Result<vk::Device>;

    ///gets [graphics queue,present queue] from device, 
    export [[nodiscard]] auto
    get_queue(vk::PhysicalDevice physical_device, vk::Device device, vk::SurfaceKHR surface) noexcept -> vkl::QueueFamilyIndices{
        return  vkl::find_queue_families(physical_device, surface);
    }
}
