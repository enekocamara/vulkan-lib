module;

#include "vulkan-lib/Config.h"
export module vulkan_lib.shader;


import debug_lib.result;

namespace vkl{
    export [[nodiscard]] inline auto
        read_file(std::string filename) noexcept -> db::Result<std::vector<char>>;
    export [[nodiscard]] inline auto
        create_module(std::string filename, vk::Device device) noexcept -> db::Result<vk::ShaderModule>;
}
