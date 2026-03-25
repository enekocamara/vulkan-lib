module;
#include "Config.h"
export module vulkan_lib.vertexManager;

import vulkan_lib.scene;
import <array>;
import <expected>;
import vulkan_lib.memory;
import debug_lib.result;

export class VertexManager{
    public:
        VertexManager();
        ~VertexManager();
        auto consume(MeshType type, const std::vector<float>& vertexData) noexcept -> void;
        [[nodiscard]] auto finalize(vk::Device device, vk::PhysicalDevice physicalDevice, vk::Queue transferQueue, vk::CommandBuffer cmdBuffer) noexcept -> db::Result<db::EmptyOk>;

        vkl::Buffer m_vertex_buffer;
        std::array<uint32_t, static_cast<size_t>(MeshType::NUM)> m_offsets;
        std::array<uint32_t, static_cast<size_t>(MeshType::NUM)> m_sizes;
    private:
        uint32_t m_offset;
        vk::Device m_device;
        std::vector<float> m_lump;
};
