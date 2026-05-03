#pragma once
#include "vulkan-lib/Config.h"
#include <debug_lib/Todo.hpp>
#include <debug_lib/Result.hpp>
#include <vulkan-lib/Memory.hpp>
namespace vkl {

    class Mesh{
    public:
        Mesh(){

        }
        void append(std::span<float> values)
        {
            m_data.append_range(values);
            m_buffer_up_to_date = false;
        }
        auto get_buffer(vk::Device device, vk::PhysicalDevice physical_device, vk::Queue transferQueue, vk::CommandBuffer cmdBuffer) -> db::Result<std::optional<vkl::Buffer>>
        {
            if (m_buffer_up_to_date)
                return m_buffer;
            
            if (m_data.size() == 0){
                m_buffer = std::nullopt;
                return m_buffer;
            }
            vkl::BufferInput input_bundle;
            input_bundle.device = device;
            input_bundle.physical_device = physical_device;
            input_bundle.size = m_data.size() * sizeof(float);
            input_bundle.usage = vk::BufferUsageFlagBits::eTransferSrc;
            input_bundle.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            // vkUtil::Buffer staging_buffer = {};
            auto staging_buffer_res = vkl::create_buffer(input_bundle);
            if (!staging_buffer_res)
            {
                return db::error("Failed to create staing buffer", std::move(staging_buffer_res.error()));
            }
            vkl::Buffer staging_buffer = staging_buffer_res.value();
            auto mapRes = vkl::mapBuffer(input_bundle.device, staging_buffer, m_data.data(), 0, static_cast<uint32_t>(m_data.size() * sizeof(float)));
            if (!mapRes)
            {
                return db::error("Failed to map buffer", std::move(mapRes.error()));
            }

            vkl::BufferInput device_local_bundle;
            device_local_bundle.device = device;
            device_local_bundle.physical_device = physical_device;
            device_local_bundle.size = m_data.size() * sizeof(float);
            device_local_bundle.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
            device_local_bundle.properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
            auto vertex_buffer_res = vkl::create_buffer(device_local_bundle);
            if (!vertex_buffer_res)
            {
                return db::error("Failed to create vertex buffer", std::move(vertex_buffer_res.error()));
            }
            m_buffer = vertex_buffer_res.value();

            vkl::CopyBufferInput copy_info = {};
            copy_info.device = device;
            copy_info.queue = transferQueue;
            copy_info.cmdBuffer = cmdBuffer;
            copy_info.srcBuffer = staging_buffer.buffer;
            copy_info.dstBuffer = m_buffer.value().buffer;
            copy_info.region.dstOffset = 0;
            copy_info.region.srcOffset = 0;
            copy_info.region.size = m_data.size() * sizeof(float);

            auto copyRes = vkl::copyBuffer(copy_info);
            if (!copyRes)
            {
                return db::error("Failed to copy buffer", std::move(copyRes.error()));
            }
            device.destroyBuffer(staging_buffer.buffer);
            device.freeMemory(staging_buffer.bufferMemory);
            m_buffer_up_to_date = true;
            return m_buffer;
        }
        uint32_t get_vertices_count(){
            return m_data.size() / 5;
        }
        ~Mesh(){

        }
    private:
        std::vector<float> m_data;
        std::optional<vkl::Buffer> m_buffer;
        bool m_buffer_up_to_date{false};
    };

    enum class MeshType : uint32_t{
        eTriangle,
        eQuad,
        eCube,
        eTriangleTex,
        eQuadTex,
        eCubeTex
    };
    [[nodiscard]] inline auto
    getBindingDescription(MeshType mesh_type) noexcept -> vk::VertexInputBindingDescription {
        vk::VertexInputBindingDescription bindingDescription{};
        bindingDescription.binding = 0;
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        switch (mesh_type)
        {
        case MeshType::eTriangle:
        case MeshType::eQuad:
        case MeshType::eCube:
            bindingDescription.stride = 3 * sizeof(float); // position only
            break;
        case MeshType::eTriangleTex:
        case MeshType::eQuadTex:
        case MeshType::eCubeTex:
            bindingDescription.stride = 5 * sizeof(float); // vec3 + vec2
            break;
        default:
            std::unreachable();
        }

        return bindingDescription;
    }


    [[nodiscard]] inline auto
    getVertexData(MeshType mesh_type) noexcept -> std::vector<float>{
        switch (mesh_type){
        case MeshType::eTriangle:
            return {
                0.0f, -0.05f, -1.0f,
                0.05f, 0.05f, -1.0f,
                -0.05f, 0.05f, -1.0f
            };

        case MeshType::eQuad:
            db::todo();
        case MeshType::eCube:
            db::todo();
        case MeshType::eTriangleTex:
            db::todo();
        case MeshType::eQuadTex:
           return{
                // positions        // UVs
                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom-left
                0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom-right
                0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top-right

                -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom-left
                0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top-right
                -0.5f, 0.5f, 0.0f, 0.0f, 1.0f   // top-left
            };
        case MeshType::eCubeTex:
            return {
                // positions          // UVs

                // LEFT (-X)
                -0.5f,-0.5f,-0.5f,   0.0f,0.0f,
                -0.5f, 0.5f, 0.5f,   1.0f,1.0f,
                -0.5f,-0.5f, 0.5f,   1.0f,0.0f,

                -0.5f,-0.5f,-0.5f,   0.0f,0.0f,
                -0.5f, 0.5f,-0.5f,   0.0f,1.0f,
                -0.5f, 0.5f, 0.5f,   1.0f,1.0f,


                // RIGHT (+X)
                 0.5f,-0.5f, 0.5f,   0.0f,0.0f,
                 0.5f, 0.5f,-0.5f,   1.0f,1.0f,
                 0.5f,-0.5f,-0.5f,   1.0f,0.0f,

                 0.5f,-0.5f, 0.5f,   0.0f,0.0f,
                 0.5f, 0.5f, 0.5f,   0.0f,1.0f,
                 0.5f, 0.5f,-0.5f,   1.0f,1.0f,


                // TOP (+Y)
                -0.5f, 0.5f, 0.5f,   0.0f,0.0f,
                 0.5f, 0.5f,-0.5f,   1.0f,1.0f,
                 0.5f, 0.5f, 0.5f,   1.0f,0.0f,

                -0.5f, 0.5f, 0.5f,   0.0f,0.0f,
                -0.5f, 0.5f,-0.5f,   0.0f,1.0f,
                 0.5f, 0.5f,-0.5f,   1.0f,1.0f,


                // BOTTOM (-Y)
                -0.5f,-0.5f,-0.5f,   0.0f,0.0f,
                 0.5f,-0.5f, 0.5f,   1.0f,1.0f,
                 0.5f,-0.5f,-0.5f,   1.0f,0.0f,

                -0.5f,-0.5f,-0.5f,   0.0f,0.0f,
                -0.5f,-0.5f, 0.5f,   0.0f,1.0f,
                 0.5f,-0.5f, 0.5f,   1.0f,1.0f,

                // FRONT (+Z)
                -0.5f,-0.5f, 0.5f,   0.0f,0.0f,
                 0.5f, 0.5f, 0.5f,   1.0f,1.0f,
                 0.5f,-0.5f, 0.5f,   1.0f,0.0f,
                        
                -0.5f,-0.5f, 0.5f,   0.0f,0.0f,
                -0.5f, 0.5f, 0.5f,   0.0f,1.0f,
                 0.5f, 0.5f, 0.5f,   1.0f,1.0f,
                        
                        
                // BACK (-Z)
                 0.5f,-0.5f,-0.5f,   0.0f,0.0f,
                -0.5f, 0.5f,-0.5f,   1.0f,1.0f,
                -0.5f,-0.5f,-0.5f,   1.0f,0.0f,

                 0.5f,-0.5f,-0.5f,   0.0f,0.0f,
                 0.5f, 0.5f,-0.5f,   0.0f,1.0f,
                -0.5f, 0.5f,-0.5f,   1.0f,1.0f,
            };
        default:
            std::unreachable();
        }
        
    }



    [[nodiscard]] inline auto
    getAttributeDescription(MeshType mesh_type) noexcept -> std::vector<vk::VertexInputAttributeDescription> {
    
        std::vector<vk::VertexInputAttributeDescription> attributes;
        switch (mesh_type)
        {
        case MeshType::eTriangle:
        case MeshType::eQuad:
        case MeshType::eCube:
            attributes.resize(1);
            attributes[0].binding = 0;
            attributes[0].location = 0;
            attributes[0].format = vk::Format::eR32G32Sfloat;
            attributes[0].offset = 0;
            break;
        case MeshType::eTriangleTex:
        case MeshType::eQuadTex:
        case MeshType::eCubeTex:
            attributes.resize(2);

            // position
            attributes[0].binding = 0;
            attributes[0].location = 0;
            attributes[0].format = vk::Format::eR32G32B32Sfloat;
            attributes[0].offset = 0;

            // UV
            attributes[1].binding = 0;
            attributes[1].location = 1;
            attributes[1].format = vk::Format::eR32G32Sfloat;
            attributes[1].offset = 3 * sizeof(float);
            break;
        default:
            std::unreachable();
        }
        return attributes;
    }


    [[nodiscard]] inline auto
    getPosColorBindingDescription() noexcept -> vk::VertexInputBindingDescription {
        vk::VertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = 3 * sizeof(float);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }

    [[nodiscard]] inline auto
    getPosColorAttributeDescriptions()noexcept -> std::vector<vk::VertexInputAttributeDescription> {
        std::vector<vk::VertexInputAttributeDescription> attributes;
        attributes.resize(1);

        attributes[0].binding = 0;
        attributes[0].location = 0;
        attributes[0].format = vk::Format::eR32G32Sfloat;
        attributes[0].offset = 0;

        return attributes;
    }
}
