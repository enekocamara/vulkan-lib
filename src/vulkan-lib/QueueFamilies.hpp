#pragma once
#include "vulkan-lib/Config.h"
#include <optional>
#include <cstdint>


#include "debug_lib/Logger.hpp"

namespace vkl {

    ///stores the queue family indices. option none if there were
    ///no suitable queue families
    struct QueueFamilyIndices{
        std::optional<uint32_t> graphics_family;
        std::optional<uint32_t> present_family;
        std::optional<uint32_t> transfer_family;
        std::optional<uint32_t> compute_family;

        ///check if both are presenst
        bool is_complete();
    };

    struct Queue{
        vk::Queue queue;
        uint32_t queueFamilyIndex;
    };
    

    [[nodiscard]] auto
        find_queue_families(vk::PhysicalDevice device, vk::SurfaceKHR surface) noexcept -> QueueFamilyIndices;
}
