module;

#include "vulkan-lib/Config.h"
#include <expected>

module vulkan_lib.vertexManager;


import debug_lib.result;

VertexManager::VertexManager(){
    m_offset = 0;
}

VertexManager::~VertexManager(){
    m_device.destroyBuffer(m_vertex_buffer.buffer);
    m_device.freeMemory(m_vertex_buffer.bufferMemory);
}

void VertexManager::consume(MeshType type, const std::vector<float>& vertex_data) noexcept{
    m_lump.insert(m_lump.end(), vertex_data.cbegin(), vertex_data.cend());
    uint32_t vertex_count = static_cast<uint32_t>(vertex_data.size()) / 7;
    m_offsets[static_cast<size_t>(type)] = m_offset;
    m_sizes[static_cast<size_t>(type)] = vertex_count;
    m_offset += vertex_count;
}
    
auto VertexManager::finalize(vk::Device device, vk::PhysicalDevice physical_device, vk::Queue transferQueue, vk::CommandBuffer cmdBuffer) noexcept->db::Result<db::EmptyOk> {
    m_device = device;
    vkl::BufferInput input_bundle;
    input_bundle.device = device;
    input_bundle.physical_device = physical_device;
    input_bundle.size = m_lump.size() * sizeof(float);
    input_bundle.usage = vk::BufferUsageFlagBits::eTransferSrc;
    input_bundle.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    //vkUtil::Buffer staging_buffer = {};
    auto staging_buffer_res = vkl::create_buffer(input_bundle);
    if (!staging_buffer_res) {
        return db::error("Failed to create staing buffer", std::move(staging_buffer_res.error()));
    }
    vkl::Buffer staging_buffer = staging_buffer_res.value();
    auto mapRes = vkl::mapBuffer(input_bundle.device, staging_buffer, m_lump.data(), 0, static_cast<uint32_t>(m_lump.size() * sizeof(float)));
    if (!mapRes) {
        return db::error("Failed to map buffer", std::move(mapRes.error()));
    }
    
    vkl::BufferInput device_local_bundle;
    device_local_bundle.device = device;
    device_local_bundle.physical_device = physical_device;
    device_local_bundle.size = m_lump.size() * sizeof(float);
    device_local_bundle.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    device_local_bundle.properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    auto vertex_buffer_res = vkl::create_buffer(device_local_bundle);
    if (!vertex_buffer_res) {
        return db::error("Failed to create vertex buffer", std::move(vertex_buffer_res.error()));
    }
    m_vertex_buffer = vertex_buffer_res.value();

    vkl::CopyBufferInput copy_info = {};
    copy_info.device = device;
    copy_info.queue = transferQueue;
    copy_info.cmdBuffer = cmdBuffer;
    copy_info.srcBuffer = staging_buffer.buffer;
    copy_info.dstBuffer = m_vertex_buffer.buffer;
    copy_info.region.dstOffset = 0;
    copy_info.region.srcOffset = 0;
    copy_info.region.size = m_lump.size() * sizeof(float);

    auto copyRes = vkl::copyBuffer(copy_info);
    if (!copyRes) {
        return db::error("Failed to copy buffer", std::move(copyRes.error()));
    }
    device.destroyBuffer(staging_buffer.buffer);
    device.freeMemory(staging_buffer.bufferMemory);
    return db::EmptyOk{};
}
