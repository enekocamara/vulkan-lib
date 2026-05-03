#pragma once

#include "vulkan-lib/Config.h"
#include <cstdint>
#include <debug_lib/Result.hpp>

namespace vkl{
    
    struct DescriptorSetLayoutData{
        uint32_t count;
        std::vector<int> indices;
        std::vector<vk::DescriptorType> types;
        std::vector<int>counts;
        std::vector<vk::ShaderStageFlags> stages;
    };

    auto
        make_descriptor_set_layout(vk::Device device, const DescriptorSetLayoutData& bindings) -> db::Result<vk::DescriptorSetLayout>;

    auto
        make_descriptor_pool(vk::Device device, uint32_t size, const std::vector<DescriptorSetLayoutData>& layouts) -> db::Result<vk::DescriptorPool>;

    auto
        allocate_descriptor_sets(vk::Device device, vk::DescriptorPool descriptorPool, const std::vector<vk::DescriptorSetLayout>& layouts) -> db::Result<std::vector<vk::DescriptorSet>>;
}
