module;
#include <vulkan-lib/Config.h>
#include <fstream>
module vulkan_lib.shader;

import debug_lib.result;

namespace vkl {

    auto read_file(std::string filename) noexcept -> db::Result<std::vector<char>> {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open())
            return db::error(std::format("Failed to open file {}", filename));
        size_t filesize = static_cast<size_t>(file.tellg());
        std::vector<char> content(filesize);
        file.seekg(0);
        file.read(content.data(), filesize);
        file.close();
        return content;
    }

    auto create_module(std::string filename, vk::Device device) noexcept -> db::Result<vk::ShaderModule> {
        auto contentsR = read_file(filename);
        if (!contentsR)
            return db::error("Failed to create shader module", contentsR.error());
        std::vector<char> contents = contentsR.value();
        vk::ShaderModuleCreateInfo createInfo = {};
        createInfo.flags = vk::ShaderModuleCreateFlags();
        createInfo.codeSize = contents.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(contents.data());
        vk::ResultValue<vk::ShaderModule> moduleR = device.createShaderModule(createInfo);
        if (moduleR.result != vk::Result::eSuccess)
            return db::error(std::format("Failed to create shader module from file {}", filename));
        return moduleR.value;
    }
}
