module;

#include "vulkan-lib/Config.h"
#include <cstdint>
export module vulkan_lib.descriptors;
import debug_lib.result;


namespace vkl{
    
    export struct DescriptorSetLayoutData{
        uint32_t count;
        std::vector<int> indices;
        std::vector<vk::DescriptorType> types;
        std::vector<int>counts;
        std::vector<vk::ShaderStageFlags> stages;
    };

    export auto
        make_descriptor_set_layout(vk::Device device, const DescriptorSetLayoutData& bindings) -> db::Result<vk::DescriptorSetLayout>;

    export inline auto
        make_descriptor_pool(vk::Device device, uint32_t size, const std::vector<DescriptorSetLayoutData>& layouts) -> db::Result<vk::DescriptorPool>;

    export auto
        allocate_descriptor_sets(vk::Device device, vk::DescriptorPool descriptorPool, const std::vector<vk::DescriptorSetLayout>& layouts) -> db::Result<std::vector<vk::DescriptorSet>>;
}
