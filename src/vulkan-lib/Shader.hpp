#pragma once

#include "vulkan-lib/Config.h"
#include "debug_lib/result.hpp"

namespace vkl{
    [[nodiscard]] inline auto
        read_file(std::string filename) noexcept -> db::Result<std::vector<char>>;
    [[nodiscard]] inline auto
        create_module(std::string filename, vk::Device device) noexcept -> db::Result<vk::ShaderModule>;
}
