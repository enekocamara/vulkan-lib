module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.descriptors;
import <vector>;
import <expected>;
import vulkan_lib.result;
import vulkan_lib.logging;

namespace vkInit{
    
    export struct DescriptorSetLayoutData{
        int count;
        std::vector<int> indices;
        std::vector<vk::DescriptorType> types;
        std::vector<int>counts;
        std::vector<vk::ShaderStageFlags> stages;
    };

    export [[nodiscard]] inline auto
    make_descriptor_set_layout(vk::Device device, const DescriptorSetLayoutData& bindings) -> std::expected<vk::DescriptorSetLayout, EmptyErr> {
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
            errprintDebug("failed to create descriptor set layout");
            return std::unexpected(EmptyErr{});
        }
        return std::move(result.value);
    }
    export [[nodiscard]] inline auto
    make_descriptor_pool(vk::Device device, uint32_t size, const DescriptorSetLayoutData& bindings) noexcept -> std::expected<vk::DescriptorPool, EmptyErr> {
        std::vector<vk::DescriptorPoolSize> poolSizes;
        for (int i = 0; i < bindings.count; i++){
            vk::DescriptorPoolSize poolSize = {};
            poolSize.type = bindings.types[i];
            poolSize.descriptorCount = size;
            poolSizes.push_back(poolSize);
        }
        vk::DescriptorPoolCreateInfo poolInfo = {};
        poolInfo.flags = vk::DescriptorPoolCreateFlags();
        poolInfo.maxSets = size;
        poolInfo.poolSizeCount  =static_cast<uint32_t>(poolSizes.size());
        poolInfo.pPoolSizes = poolSizes.data();

        vk::ResultValue<vk::DescriptorPool> result = device.createDescriptorPool(poolInfo);
        if (result.result != vk::Result::eSuccess){
            errprintDebug("failed to create descriptor pool");
            return std::unexpected(EmptyErr{});
        }
        return std::move(result.value);
    }

    export [[nodiscard]] inline auto
    allocate_descriptor_set(vk::Device device, vk::DescriptorPool descriptorPool, vk::DescriptorSetLayout layout) noexcept -> std::expected<vk::DescriptorSet, EmptyErr> {
        vk::DescriptorSetAllocateInfo allocInfo = {};
        allocInfo.descriptorPool = descriptorPool;
        allocInfo.descriptorSetCount = 1;
        allocInfo.pSetLayouts = &layout;

        vk::ResultValue<std::vector<vk::DescriptorSet>> result = device.allocateDescriptorSets(allocInfo);
        if (result.result != vk::Result::eSuccess){
            errprintDebug("failed to allocate descriptor");
            return std::unexpected(EmptyErr{});
        }
        return std::move(result.value[0]);
    }
}
