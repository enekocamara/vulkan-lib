module;
#include "Config.h"
export module vulkan_lib.vertexManager;

import vulkan_lib.scene;
import <array>;
import <expected>;
import vulkan_lib.memory;
import vulkan_lib.result;

export class VertexManager{
    public:
        VertexManager();
        ~VertexManager();
        void consume(MeshType type, const std::vector<float>& vertexData) noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> finalize(vk::Device device, vk::PhysicalDevice physicalDevice, vk::Queue transferQueue, vk::CommandBuffer cmdBuffer) noexcept;
        vkUtil::Buffer vertexBuffer;
        std::array<uint32_t, static_cast<size_t>(MeshType::NUM)> offsets;
        std::array<uint32_t, static_cast<size_t>(MeshType::NUM)> sizes;
    private:
        uint32_t offset;
        vk::Device device;
        std::vector<float> lump;
};
