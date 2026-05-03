#pragma once
#include "vulkan-lib/Config.h"

#include "debug_lib/result.hpp"

namespace vkl {
    auto
        make_semaphore(vk::Device device) noexcept -> db::Result<vk::Semaphore>;
    auto
        make_fence(vk::Device device) noexcept -> db::Result<vk::Fence>;
}

