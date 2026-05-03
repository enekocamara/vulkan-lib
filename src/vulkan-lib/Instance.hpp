#pragma once

#include "vulkan-lib/Config.h"
#include <vector>
#include <string>

#include "debug_lib/Logger.hpp"
#include "debug_lib/result.hpp"



namespace vkl{

    [[nodiscard]] auto
        supported(std::vector<const char*>& extensions, std::vector<const char*>& layers) -> bool;

    auto
        make_instance(const std::string& app_name, const std::string& engine_name) -> db::Result<vk::Instance>;
}
