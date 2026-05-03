#pragma once

#include "vulkan-lib/Config.h"
#include <glm/glm.hpp>

#include "vulkan-lib/memory.hpp"
#include "debug_lib/result.hpp"

namespace vkl {

    struct UBO{
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewProjection;
    };

    class SwapchainFrame {
    public:
        struct CreateInfo
        {
            vk::Device device;
            vk::PhysicalDevice physical_device;
            vk::Image image;
            vk::Format format;
            vk::CommandBuffer command_buffer;
            std::vector<vk::DescriptorSet> descriptor_sets;
            uint32_t width;
            uint32_t height;
        };
        SwapchainFrame() = delete;
        SwapchainFrame(const SwapchainFrame::CreateInfo& info);

        auto get_command_buffer() -> vk::CommandBuffer { return m_command_buffer; }

        vk::Semaphore m_image_available;
        vk::Semaphore m_render_finished;
        vk::Fence m_in_flight_fence;

        vk::Image m_image;
        vk::ImageView m_view;
        vk::Framebuffer m_framebuffer;

        vk::Image m_depth_image;
        vk::ImageView m_depth_view;
        vk::CommandBuffer m_command_buffer;

        vk::DescriptorBufferInfo m_uniform_buffer_descriptor;
        vk::DescriptorBufferInfo m_model_buffer_descriptor;
        std::vector<vk::DescriptorSet> m_descriptor_sets;
    private:


        //sync
    };
/*
    struct SwapchainFrame2{
        //sync
        vk::Semaphore imageAvailable, renderFinished;
        vk::Fence inFlightFence;

        vk::DescriptorBufferInfo uniformBufferDescriptor;
        vk::DescriptorBufferInfo modelBufferDescriptor;
        std::vector<vk::DescriptorSet> descriptor_sets;

        UBO cameraData;
        vkl::Buffer cameraDataBuffer;
        void *cameraDataWriteLocation;
        std::vector<glm::mat4>modelTransforms;
        vkl::Buffer modelBuffer;
        void *modelBufferWriteLocation;

        std::expected<EmptyOk, EmptyErr> make_descriptor_resources(vk::Device device, vk::PhysicalDevice physicalDevice){
            //camera

            vkl::BufferInput input = {};
            input.device = device;
            input.physicalDevice = physicalDevice;
            input.properties = vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent;
            input.size = sizeof(UBO);
            input.usage = vk::BufferUsageFlagBits::eUniformBuffer;
            auto camBuffRes = vkl::create_buffer(input);
            if (!camBuffRes) {
                return std::unexpected(EmptyErr{});
            }
            cameraDataBuffer = camBuffRes.value();

            vk::ResultValue<void *> result = device.mapMemory(cameraDataBuffer.bufferMemory, 0, sizeof(UBO));
            if (result.result != vk::Result::eSuccess){
                //errprintDebug("Failed to map memory");
                return std::unexpected(EmptyErr{});
            }
            
            cameraDataWriteLocation = result.value;
            uniformBufferDescriptor.buffer = cameraDataBuffer.buffer;
            uniformBufferDescriptor.offset = 0;
            uniformBufferDescriptor.range = sizeof(UBO);

            //model

            input.size = 1024 * sizeof(glm::mat4);
            input.usage = vk::BufferUsageFlagBits::eStorageBuffer;
            auto modelBuffRes = vkl::create_buffer(input);
            if (!modelBuffRes) {
                return std::unexpected(EmptyErr{});
            }
            modelBuffer = modelBuffRes.value();
            vk::ResultValue<void *> resultModel = device.mapMemory(modelBuffer.bufferMemory, 0, sizeof(UBO));
            if (resultModel.result != vk::Result::eSuccess){
               // errprintDebug("Failed to map memory");
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
            writeInfo.dstSet = descriptor_sets[0];
            writeInfo.dstBinding = 0;
            writeInfo.descriptorCount = 1;
            writeInfo.descriptorType = vk::DescriptorType::eUniformBuffer;
            writeInfo.pBufferInfo = &uniformBufferDescriptor;
            device.updateDescriptorSets(writeInfo, nullptr);
            
            vk::WriteDescriptorSet writeInfo2 = {};
            writeInfo2.dstSet = descriptor_sets[0];
            writeInfo2.dstBinding = 1;
            writeInfo2.descriptorCount = 1;
            writeInfo2.descriptorType = vk::DescriptorType::eStorageBuffer;
            writeInfo2.pBufferInfo = &modelBufferDescriptor;
            device.updateDescriptorSets(writeInfo2, nullptr);

        }
    };*/
}
