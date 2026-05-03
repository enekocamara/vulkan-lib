#pragma once
#include "vulkan-lib/Config.h"
#include <cstdint>

#include "debug_lib/result.hpp"


namespace vkl {

    struct BufferInput {
        vk::Device device;
        vk::PhysicalDevice physical_device;
        size_t size;
        vk::BufferUsageFlags usage;
        vk::MemoryPropertyFlags properties;
    };


    struct Buffer {
        enum State {
            Init,
            BufferCreated,
            MemAllocated,
            MemBinded,
            Mapped,
            Empty,
            Err,
        };
        State state;

        vk::Buffer buffer;
        vk::DeviceMemory bufferMemory;

        static Buffer init() {
            Buffer buffer = {};
            buffer.state = State::Init;
            return {};
        }
    };

    struct CopyBufferInput {
        vk::Device device;
        vk::Queue queue;
        vk::CommandBuffer cmdBuffer;
        vk::Buffer dstBuffer;
        vk::Buffer srcBuffer;
        vk::BufferCopy region;
    };

    auto
        find_memory_type_index(vk::PhysicalDevice physicalDevice, uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties)noexcept -> db::Result<uint32_t>;

    auto
        allocate_buffer_memory(Buffer& buffer, const BufferInput& input) noexcept -> db::Result<db::EmptyOk>;

    auto
        create_buffer(BufferInput& bufferInput) noexcept -> db::Result<Buffer>;

    auto
        mapBuffer(vk::Device device, Buffer& buffer, void* src, uint32_t offset, uint32_t size) -> db::Result<db::EmptyOk>;


    auto
        copyBuffer(CopyBufferInput input) -> db::Result<db::EmptyOk>;
}
