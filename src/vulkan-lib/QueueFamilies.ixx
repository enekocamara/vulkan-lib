module;

#include "vulkan-lib/Config.h"
#include <optional>
#include <cstdint>
export module vulkan_lib.queue_families;


import debug_lib.Logger;

export namespace vkl {

    ///stores the queue family indices. option none if there were
    ///no suitable queue families
    export struct QueueFamilyIndices{
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> transferFamily;

        ///check if both are presenst
        bool is_complete();
    };

    export struct Queue{
        vk::Queue queue;
        uint32_t queueFamilyIndex;
    };
    

    export [[nodiscard]] inline auto
        find_queue_families(vk::PhysicalDevice& device, vk::SurfaceKHR surface) noexcept -> QueueFamilyIndices;
}
