module;

#include "vulkan-lib/Config.h"
#include <fstream>
export module vulkan_lib.shader;

import <iostream>;
import vulkan_lib.result;
import <expected>;

namespace vkInit{
    export [[nodiscard]] inline auto
    read_file(std::string filename) noexcept -> std::expected<std::vector<char>, EmptyErr> {
        std::ifstream file(filename, std::ios::ate | std::ios::binary);
        if (!file.is_open()){
            if constexpr(_DEBUG)
                std::cout << "failed to open file " << filename << std::endl;
            return std::unexpected(EmptyErr{});
        }
        size_t filesize = static_cast<size_t>(file.tellg());
        std::vector<char> content(filesize);
        file.seekg(0);
        file.read(content.data(), filesize);
        file.close();
        return content;
    }

    export [[nodiscard]] inline auto
    create_module(std::string filename, vk::Device device) noexcept -> std::expected<vk::ShaderModule, EmptyErr> {
        std::expected<std::vector<char>, EmptyErr> contentsR = read_file(filename);
        if (!contentsR)
            return std::unexpected(EmptyErr{});
        std::vector<char> contents = contentsR.value();
        vk::ShaderModuleCreateInfo createInfo = {};
        createInfo.flags = vk::ShaderModuleCreateFlags();
        createInfo.codeSize = contents.size();
        createInfo.pCode = reinterpret_cast<const uint32_t *>(contents.data());
        vk::ResultValue<vk::ShaderModule> moduleR = device.createShaderModule(createInfo);
        if (moduleR.result != vk::Result::eSuccess){
            if constexpr(_DEBUG)
                std::cerr << "afiled to create shader module with file " << filename << '\n';
            return std::unexpected(EmptyErr{});
        }
        return moduleR.value;
    }
}
