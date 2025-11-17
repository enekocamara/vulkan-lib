module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.memory;

import vulkan_lib.result;
import <expected>;
import <iostream>;

export namespace vkUtil{

    export struct BufferInput{
        vk::Device device;
        vk::PhysicalDevice physicalDevice;
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

    export [[nodiscard]] inline std::expected<uint32_t, EmptyErr > findMemoryTypeIndex(vk::PhysicalDevice physicalDevice,  uint32_t supportedMemoryIndices, vk::MemoryPropertyFlags requestedProperties)noexcept{
        vk::PhysicalDeviceMemoryProperties memoryProperties;
        physicalDevice.getMemoryProperties(&memoryProperties);

        for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++){
            bool supported = (supportedMemoryIndices & (1 << i)) != 0;
            bool sufficient{(memoryProperties.memoryTypes[i].propertyFlags & requestedProperties) == requestedProperties};
            if (supported && sufficient)
                return i;
        }
        return std::unexpected(EmptyErr{});
    }
    
    export [[nodiscard]] inline auto
    allocateBufferMemory( Buffer&buffer, const BufferInput& input) noexcept -> std::expected<EmptyOk, EmptyErr> {
        assert(buffer.state == Buffer::State::BufferCreated);

        vk::MemoryRequirements memoryRequirements;
        input.device.getBufferMemoryRequirements(buffer.buffer, &memoryRequirements);
        vk::MemoryAllocateInfo allocInfo = {};
        allocInfo.allocationSize = memoryRequirements.size;
        auto memoryTypeIndexRes = findMemoryTypeIndex(input.physicalDevice, memoryRequirements.memoryTypeBits,
            vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
        if (!memoryTypeIndexRes)
            return std::unexpected(EmptyErr{});
        allocInfo.memoryTypeIndex = memoryTypeIndexRes.value();
        vk::DeviceMemory memory;
        vk::Result allocRes = input.device.allocateMemory(&allocInfo, nullptr, &buffer.bufferMemory);
        //vk::ResultValue<vk::DeviceMemory> memoryR = a;
        if (allocRes != vk::Result::eSuccess){
            input.device.destroyBuffer(buffer.buffer, nullptr);
            buffer.state = Buffer::State::Err;
            return std::unexpected(EmptyErr{});
        }
        buffer.state = Buffer::MemAllocated;
        if (input.device.bindBufferMemory(buffer.buffer, buffer.bufferMemory, 0) != vk::Result::eSuccess){
            input.device.freeMemory(buffer.bufferMemory);
            input.device.destroyBuffer(buffer.buffer);
            buffer.state = Buffer::Err;
            return std::unexpected(EmptyErr{});
        }
        buffer.state = Buffer::MemBinded;
        return EmptyOk{};
    }

    export [[nodiscard]] inline auto
    createBuffer(BufferInput& bufferInput) noexcept -> std::expected<Buffer, EmptyErr> {
        vk::BufferCreateInfo bufferInfo = {};
        bufferInfo.flags = vk::BufferCreateFlags();
        bufferInfo.size = bufferInput.size;
        bufferInfo.usage = bufferInput.usage;
        bufferInfo.sharingMode = vk::SharingMode::eExclusive;

        Buffer buffer = Buffer::init();
        if (bufferInput.device.createBuffer(&bufferInfo,nullptr, &buffer.buffer) != vk::Result::eSuccess){
            if constexpr(_DEBUG)
                std::cerr << "failed to create buffer\n";
            return std::unexpected(EmptyErr{});
        }
        buffer.state = Buffer::State::BufferCreated;
        auto res = allocateBufferMemory(buffer, bufferInput);
        if (!res) {
            return std::unexpected(res.error());
        }
        return buffer;
    }

    export [[nodiscard]] inline auto
    mapBuffer(vk::Device device, Buffer& buffer, void *src, uint32_t offset, uint32_t size) -> std::expected<EmptyOk, EmptyErr> {
        assert(buffer.state == Buffer::MemAllocated || buffer.state == Buffer::MemBinded);
        vk::ResultValue<void *> memoryLocationR = device.mapMemory(buffer.bufferMemory, offset, size);
        if (memoryLocationR.result != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        memcpy(memoryLocationR.value, src, size);
        device.unmapMemory(buffer.bufferMemory);
        return EmptyOk{};
    }


    export [[nodiscard]] inline auto
    copyBuffer(CopyBufferInput input) -> std::expected<EmptyOk, EmptyErr> {
        if (input.cmdBuffer.reset() != vk::Result::eSuccess){
            if constexpr(_DEBUG)
                std::cerr << "failed to reset cmdbuffer\n";
            return std::unexpected(EmptyErr{});
        }
        
        vk::CommandBufferBeginInfo info = {};
        info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        if (input.cmdBuffer.begin(info) != vk::Result::eSuccess){
            if constexpr(_DEBUG)
                std::cerr << "failed to begin command buffer\n";
            return std::unexpected(EmptyErr{});
        }
        input.cmdBuffer.copyBuffer(input.srcBuffer, input.dstBuffer, 1, &input.region); 
        if (input.cmdBuffer.end() != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        vk::SubmitInfo submitInfo ={};
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &input.cmdBuffer;
        if (input.queue.submit(1, &submitInfo, nullptr) != vk::Result::eSuccess){
            if constexpr (_DEBUG)
                std::cerr << "failed to submit cmdbuffer\n";
            return std::unexpected(EmptyErr{});
        }
        if (input.queue.waitIdle() != vk::Result::eSuccess){
            if constexpr(_DEBUG)
                std::cerr << "failed to wait for transfer queue to finish\n";
            return std::unexpected(EmptyErr{});
        }
        return EmptyOk{};
    }
}
