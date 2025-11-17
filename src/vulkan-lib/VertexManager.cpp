module;

#include "vulkan-lib/Config.h"

module vulkan_lib.vertexManager;

import <expected>;

VertexManager::VertexManager(){
    offset = 0;
}

VertexManager::~VertexManager(){
    device.destroyBuffer(vertexBuffer.buffer);
    device.freeMemory(vertexBuffer.bufferMemory);
}
void VertexManager::consume(MeshType type, const std::vector<float>& vertexData) noexcept{
    lump.insert(lump.end(), vertexData.cbegin(), vertexData.cend());
    uint32_t vertexCount = static_cast<uint32_t>(vertexData.size()) / 7;
    offsets[static_cast<size_t>(type)] = offset;
    sizes[static_cast<size_t>(type)] = vertexCount;
    offset += vertexCount;
}
    
[[nodiscard]] std::expected<EmptyOk, EmptyErr> VertexManager::finalize(vk::Device device, vk::PhysicalDevice physicalDevice, vk::Queue transferQueue, vk::CommandBuffer cmdBuffer) noexcept{
    this->device = device;
    vkUtil::BufferInput inputBundle;
    inputBundle.device = device;
    inputBundle.physicalDevice = physicalDevice;
    inputBundle.size = lump.size() * sizeof(float);
    inputBundle.usage = vk::BufferUsageFlagBits::eTransferSrc;
    inputBundle.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
    //vkUtil::Buffer stagingBuffer = {};
    auto stagingBufferRes = vkUtil::createBuffer(inputBundle);
    if (!stagingBufferRes) {
        return std::unexpected(EmptyErr{});
    }
    vkUtil::Buffer stagingBuffer = stagingBufferRes.value();
    auto mapRes = vkUtil::mapBuffer(inputBundle.device, stagingBuffer, lump.data(), 0, static_cast<uint32_t>(lump.size() * sizeof(float)));
    if (!mapRes) {
        return std::unexpected(EmptyErr{});
    }
    
    vkUtil::BufferInput deviceLocalBundle;
    deviceLocalBundle.device = device;
    deviceLocalBundle.physicalDevice = physicalDevice;
    deviceLocalBundle.size = lump.size() * sizeof(float);
    deviceLocalBundle.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eTransferDst;
    deviceLocalBundle.properties = vk::MemoryPropertyFlagBits::eDeviceLocal;
    auto vertexBufferRes = vkUtil::createBuffer(deviceLocalBundle);
    if (!vertexBufferRes) {
        return std::unexpected(EmptyErr{});
    }
    vertexBuffer = vertexBufferRes.value();

    vkUtil::CopyBufferInput copyInfo = {};
    copyInfo.device = device;
    copyInfo.queue = transferQueue;
    copyInfo.cmdBuffer = cmdBuffer;
    copyInfo.srcBuffer = stagingBuffer.buffer;
    copyInfo.dstBuffer = vertexBuffer.buffer;
    copyInfo.region.dstOffset = 0;
    copyInfo.region.srcOffset = 0;
    copyInfo.region.size = lump.size() * sizeof(float);

    auto copyRes = vkUtil::copyBuffer(copyInfo);
    if (!copyRes) {
        return std::unexpected(EmptyErr{});
    }
    device.destroyBuffer(stagingBuffer.buffer);
    device.freeMemory(stagingBuffer.bufferMemory);
    return EmptyOk{};
}
