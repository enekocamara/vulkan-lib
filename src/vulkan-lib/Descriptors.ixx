module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.descriptors;
import <vector>;
import <expected>;
import debug_lib.result;
import debug_lib.Logger;

namespace vkl{
    
    export struct DescriptorSetLayoutData{
        uint32_t count;
        std::vector<int> indices;
        std::vector<vk::DescriptorType> types;
        std::vector<int>counts;
        std::vector<vk::ShaderStageFlags> stages;
    };

    export [[nodiscard]] auto
        make_descriptor_set_layout(vk::Device device, const DescriptorSetLayoutData& bindings) -> db::Result<vk::DescriptorSetLayout>;
    export [[nodiscard]] inline auto
        make_descriptor_pool(vk::Device device, uint32_t size, const std::vector<DescriptorSetLayoutData>& layouts) noexcept -> db::Result<vk::DescriptorPool>;

    export [[nodiscard]] inline auto
    allocate_descriptor_sets(vk::Device device, vk::DescriptorPool descriptorPool, const std::vector<vk::DescriptorSetLayout>& layouts) noexcept -> db::Result<std::vector<vk::DescriptorSet>> {
        vk::DescriptorSetAllocateInfo allocInfo = {};
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = layouts.size();
        allocInfo.pSetLayouts = layouts.data();

        vk::ResultValue<std::vector<vk::DescriptorSet>> result = device.allocateDescriptorSets(allocInfo);
        if (result.result != vk::Result::eSuccess){
            db::Logger::core_error("failed to allocate descriptor");
            return db::error("Failed to allocate descriptor");
        }
        return std::move(result.value);
    }
}
