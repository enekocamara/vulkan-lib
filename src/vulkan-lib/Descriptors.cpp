module;

#include <vulkan-lib/Config.h>

module vulkan_lib.descriptors;

import <vector>;
import debug_lib.result;

namespace vkl {

    auto make_descriptor_set_layout(vk::Device device, const DescriptorSetLayoutData& bindings) -> db::Result<vk::DescriptorSetLayout> {
        std::vector<vk::DescriptorSetLayoutBinding> layoutBindings;
        layoutBindings.reserve(bindings.count);

        for (int i = 0; i < bindings.count; i++){
            vk::DescriptorSetLayoutBinding layoutBinding = {};
            layoutBinding.binding = bindings.indices[i];
            layoutBinding.descriptorType = bindings.types[i];
            layoutBinding.descriptorCount = bindings.counts[i];
            layoutBinding.stageFlags = bindings.stages[i];
            layoutBindings.push_back(layoutBinding);
        }
        vk::DescriptorSetLayoutCreateInfo createInfo = {};
        createInfo.flags = vk::DescriptorSetLayoutCreateFlagBits();
        createInfo.bindingCount = static_cast<uint32_t>(layoutBindings.size());
        createInfo.pBindings = layoutBindings.data();

        vk::ResultValue<vk::DescriptorSetLayout> result = device.createDescriptorSetLayout(createInfo);
        if (result.result != vk::Result::eSuccess){
            db::Logger::core_error("failed to create descriptor set layout");
            return db::error("Failed to creaet descriptor set layout");
        }
        return std::move(result.value);
    }
    
    auto make_descriptor_pool(vk::Device device, uint32_t size, const std::vector<DescriptorSetLayoutData>& layouts) noexcept -> db::Result<vk::DescriptorPool> {
        std::vector<vk::DescriptorPoolSize> poolSizes;
        for (const auto& layout : layouts) {
            for (int i = 0; i < layout.count; i++) {
                vk::DescriptorPoolSize poolSize = {};
                poolSize.type = layout.types[i];
                poolSize.descriptorCount = size;
                poolSizes.push_back(poolSize);
            }
        }
        vk::DescriptorPoolCreateInfo poolInfo = {};
        poolInfo.flags = vk::DescriptorPoolCreateFlags();
        poolInfo.maxSets = size;
        poolInfo.poolSizeCount  =static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        vk::ResultValue<vk::DescriptorPool> result = device.createDescriptorPool(poolInfo);
        if (result.result != vk::Result::eSuccess)
            return db::error("Failed to create descriptor pool");
        return std::move(result.value);
    }

}