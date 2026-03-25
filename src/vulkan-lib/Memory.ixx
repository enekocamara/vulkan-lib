module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.memory;

import debug_lib.result;
import <expected>;
import <iostream>;

export namespace vkl{

    export struct BufferInput{
        vk::Device device;
        vk::PhysicalDevice physical_device;
        size_t size;
        vk::BufferUsageFlags usage;
        vk::MemoryPropertyFlags properties;
    };


    export struct Buffer{
        enum State{
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

        static Buffer init(){
            Buffer buffer = {};
            buffer.state = State::Init;
            return {};
        }
    };
    
    export struct CopyBufferInput{
        vk::Device device;
        vk::Queue queue;
        vk::CommandBuffer cmdBuffer;
        vk::Buffer dstBuffer;
        vk::Buffer srcBuffer;
        vk::BufferCopy region;
    };

    export [[nodiscard]] auto
    findMemoryTypeIndex(vk::PhysicalDevice physicalDevice,  uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties)noexcept -> db::Result<uint32_t>{
        vk::PhysicalDeviceMemoryProperties memoryProperties;
        physicalDevice.getMemoryProperties(&memoryProperties);

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++){
            bool supported = (supportedMemoryIndices & (1 << i)) != 0;
            bool sufficient{(memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties};
            if (supported && sufficient)
                return i;
        }
        return db::error("Failed to get memory type index");
    }
    
    export [[nodiscard]] inline auto
    allocate_buffer_memory( Buffer&buffer, const BufferInput& input) noexcept -> db::Result<db::EmptyOk> {
        assert(buffer.state == Buffer::State::BufferCreated);

        vk::MemoryRequirements memoryRequirements;
        input.device.getBufferMemoryRequirements(buffer.buffer, &memoryRequirements);
        vk::MemoryAllocateInfo allocInfo = {};
        allocInfo.allocationSize = memoryRequirements.size;
        auto memory_type_index_res = findMemoryTypeIndex(input.physical_device, memoryRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        if (!memory_type_index_res)
            return db::error("Failed to find memory type index", std::move(memory_type_index_res.error()));
        allocInfo.memoryTypeIndex = memory_type_index_res.value();
        vk::DeviceMemory memory;
        vk::Result allocRes = input.device.allocateMemory(&allocInfo, nullptr, &buffer.bufferMemory);
        //vk::ResultValue<vk::DeviceMemory> memoryR = a;
        if (allocRes != vk::Result::eSuccess){
            input.device.destroyBuffer(buffer.buffer, nullptr);
            buffer.state = Buffer::State::Err;
            return db::error("Failed to allocate memory");
        }
        buffer.state = Buffer::MemAllocated;
        if (input.device.bindBufferMemory(buffer.buffer, buffer.bufferMemory, 0) != vk::Result::eSuccess){
            input.device.freeMemory(buffer.bufferMemory);
            input.device.destroyBuffer(buffer.buffer);
            buffer.state = Buffer::Err;
            return db::error("Failed to bind buffer memory");
        }
        buffer.state = Buffer::MemBinded;
        return db::EmptyOk{};
    }

    export [[nodiscard]] inline auto
    create_buffer(BufferInput& bufferInput) noexcept -> db::Result<Buffer> {
        vk::BufferCreateInfo bufferInfo = {};
        bufferInfo.flags = vk::BufferCreateFlags();
        bufferInfo.size = bufferInput.size;
        bufferInfo.usage = bufferInput.usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        Buffer buffer = Buffer::init();
        if (bufferInput.device.createBuffer(&bufferInfo,nullptr, &buffer.buffer) != vk::Result::eSuccess){
            return db::error("Failed to create buffer");
        }
        buffer.state = Buffer::State::BufferCreated;
        auto res = allocate_buffer_memory(buffer, bufferInput);
        if (!res) {
            return db::error("Failed to allocate buffer memory", std::move(res.error()));
        }
        return buffer;
    }

    export [[nodiscard]] inline auto
    mapBuffer(vk::Device device, Buffer& buffer, void *src, uint32_t offset, uint32_t size) -> db::Result<db::EmptyOk> {
        assert(buffer.state == Buffer::MemAllocated || buffer.state == Buffer::MemBinded);
        vk::ResultValue<void *> memoryLocationR = device.mapMemory(buffer.bufferMemory, offset, size);
        if (memoryLocationR.result != vk::Result::eSuccess)
            db::error("Failed to map memory");
        memcpy(memoryLocationR.value, src, size);
        device.unmapMemory(buffer.bufferMemory);
        return db::EmptyOk{};
    }


    export [[nodiscard]] inline auto
    copyBuffer(CopyBufferInput input) -> db::Result<db::EmptyOk> {
        if (input.cmdBuffer.reset() != vk::Result::eSuccess){
            return db::error("Failed to reset command buffer");
        }
        
        vk::CommandBufferBeginInfo info = {};
        info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        if (input.cmdBuffer.begin(info) != vk::Result::eSuccess){
            return db::error("Failed to begin command buffer");
        }
        input.cmdBuffer.copyBuffer(input.srcBuffer, input.dstBuffer, 1, &input.region); 
        if (input.cmdBuffer.end() != vk::Result::eSuccess)
            return db::error("Failed to copy buffer");
        vk::SubmitInfo submitInfo ={};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &input.cmdBuffer;
        if (input.queue.submit(1, &submitInfo, nullptr) != vk::Result::eSuccess){
            return db::error("Failed to submit command buffer");
        }
        if (input.queue.waitIdle() != vk::Result::eSuccess){
            return db::error("failed to wait for transfer queue to finish");
        }
        return db::EmptyOk{};
    }
}
