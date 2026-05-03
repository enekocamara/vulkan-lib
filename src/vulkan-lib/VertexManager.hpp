#pragma once

#include "Config.h"

#include "vulkan-lib/scene.hpp"
#include "vulkan-lib/memory.hpp"
#include "debug_lib/result.hpp"

class VertexManager{
    public:
        VertexManager();
        ~VertexManager();
        auto consume(MeshType type, const std::vector<float>& vertexData) noexcept -> void;
        auto finalize(vk::Device device, vk::PhysicalDevice physicalDevice, vk::Queue transferQueue, vk::CommandBuffer cmdBuffer) noexcept -> db::Result<db::EmptyOk>;

        vkl::Buffer m_vertex_buffer;
        std::vector<float> m_lump;
    private:
        vk::Device m_device;
};
