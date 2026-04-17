module;

#include "vulkan-lib/Config.h"
#include <vector>
#include <string>

export module vulkan_lib.instance;
import debug_lib.Logger;
import debug_lib.result;



namespace vkl{

    export [[nodiscard]] auto
        supported(std::vector<const char*>& extensions, std::vector<const char*>& layers) -> bool;

    export auto
        make_instance(const std::string& app_name, const std::string& engine_name) -> db::Result<vk::Instance>;
}
