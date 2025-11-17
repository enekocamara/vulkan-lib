module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.swapchainFrame;

import vulkan_lib.memory;
import <glm/glm.hpp>;
import <expected>;
import vulkan_lib.result;
import vulkan_lib.logging;

export namespace vkInit {

    struct UBO{
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;
    };

    export struct SwapchainFrame{
        vk::Image image;
        vk::ImageView view;
        vk::Framebuffer framebuffer;
        vk::CommandBuffer commandbuffer;
        //sync
        vk::Semaphore imageAvailable, renderFinished;
        vk::Fence inFlightFence;

        vk::DescriptorBufferInfo uniformBufferDescriptor;
        vk::DescriptorBufferInfo modelBufferDescriptor;
        vk::DescriptorSet descriptorSet;

        UBO cameraData;
        vkUtil::Buffer cameraDataBuffer;
        void *cameraDataWriteLocation;
        std::vector<glm::mat4>modelTransforms;
        vkUtil::Buffer modelBuffer;
        void *modelBufferWriteLocation;

        std::expected<EmptyOk, EmptyErr> make_descriptor_resources(vk::Device device, vk::PhysicalDevice physicalDevice){
            //camera

            vkUtil::BufferInput input = {};
            input.device = device;
            input.physicalDevice = physicalDevice;
            input.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            input.size = sizeof(UBO);
            input.usage = vk::BufferUsageFlagBits::eUniformBuffer;
            auto camBuffRes = vkUtil::createBuffer(input);
            if (!camBuffRes) {
                return std::unexpected(EmptyErr{});
            }
            cameraDataBuffer = camBuffRes.value();

            vk::ResultValue<void *> result = device.mapMemory(cameraDataBuffer.bufferMemory, 0, sizeof(UBO));
            if (result.result != vk::Result::eSuccess){
                errprintDebug("Failed to map memory");
                return std::unexpected(EmptyErr{});
            }
            
            cameraDataWriteLocation = result.value;
            uniformBufferDescriptor.buffer = cameraDataBuffer.buffer;
            uniformBufferDescriptor.offset = 0;
            uniformBufferDescriptor.range = sizeof(UBO);

            //model

            input.size = 1024 * sizeof(glm::mat4);
            input.usage = vk::BufferUsageFlagBits::eStorageBuffer;
            auto modelBuffRes = vkUtil::createBuffer(input);
            if (!modelBuffRes) {
                return std::unexpected(EmptyErr{});
            }
            modelBuffer = modelBuffRes.value();
            vk::ResultValue<void *> resultModel = device.mapMemory(modelBuffer.bufferMemory, 0, sizeof(UBO));
            if (resultModel.result != vk::Result::eSuccess){
                errprintDebug("Failed to map memory");
                return std::unexpected(EmptyErr{});
            }
            modelBufferWriteLocation = resultModel.value;
        
            modelTransforms.reserve(1024);
            for (uint32_t i  = 0; i < 1024; i++){
                modelTransforms.push_back(glm::mat4(1.0f));
            }

            modelBufferDescriptor.buffer = modelBuffer.buffer;
            modelBufferDescriptor.offset = 0;
            modelBufferDescriptor.range = sizeof(glm::mat4) * 1024;
            return EmptyOk{};
        }
        void write_descriptor_set(vk::Device device){
            vk::WriteDescriptorSet writeInfo = {};
            writeInfo.dstSet = descriptorSet;
            writeInfo.dstBinding = 0;
            writeInfo.descriptorCount = 1;
            writeInfo.descriptorType = vk::DescriptorType::eUniformBuffer;
            writeInfo.pBufferInfo = &uniformBufferDescriptor;
            device.updateDescriptorSets(writeInfo, nullptr);
            
            vk::WriteDescriptorSet writeInfo2 = {};
            writeInfo2.dstSet = descriptorSet;
            writeInfo2.dstBinding = 1;
            writeInfo2.descriptorCount = 1;
            writeInfo2.descriptorType = vk::DescriptorType::eStorageBuffer;
            writeInfo2.pBufferInfo = &modelBufferDescriptor;
            device.updateDescriptorSets(writeInfo2, nullptr);

        }
    };
}
