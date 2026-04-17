module;
#include "vulkan-lib/Config.h"
export module vulkan_lib.sync;

import debug_lib.result;

namespace vkl {
    export auto
        make_semaphore(vk::Device device) noexcept -> db::Result<vk::Semaphore>;
    export auto
        make_fence(vk::Device device) noexcept -> db::Result<vk::Fence>;
}

