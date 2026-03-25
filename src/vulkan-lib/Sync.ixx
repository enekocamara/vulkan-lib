module;
#include "vulkan-lib/Config.h"
export module vulkan_lib.sync;


import debug_lib.result;
import <expected>;
import <iostream>;

namespace vkl {
    export [[nodiscard]] inline auto
        make_semaphore(vk::Device device) noexcept -> db::Result<vk::Semaphore>;
    export [[nodiscard]] inline auto
        make_fence(vk::Device device) noexcept -> db::Result<vk::Fence>;
}

