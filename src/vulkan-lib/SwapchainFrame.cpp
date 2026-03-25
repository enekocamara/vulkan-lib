module;
#include "vulkan-lib/Config.h"

module vulkan_lib.SwapchainFrame;

import vulkan_lib.memory;
import <glm/glm.hpp>;
import <expected>;
import debug_lib.result;
import vulkan_lib.logging;
import vulkan_lib.sync;

namespace vkl {
    SwapchainFrame::SwapchainFrame(vk::Device device, vk::PhysicalDevice physical_device, vk::Image image, vk::Format format, vk::CommandBuffer command_buffer) :
        m_image(image),
        m_command_buffer(command_buffer)
    {
        auto frame_in_flight_fence_res = vkl::make_fence(device);
        auto frame_image_available_res = vkl::make_semaphore(device);
        auto frame_render_finished_res = vkl::make_semaphore(device);

        if (!frame_in_flight_fence_res || !frame_image_available_res || !frame_render_finished_res)
            throw std::runtime_error("Failed to creaet sync objects");
        m_in_flight_fence = frame_in_flight_fence_res.value();
        m_image_available = frame_image_available_res.value();
        m_render_finished = frame_render_finished_res.value();


        vk::ImageViewCreateInfo createInfo(
            vk::ImageViewCreateFlags(),
            m_image,
            vk::ImageViewType::e2D,
            format
        );
        createInfo.components.a = vk::ComponentSwizzle::eIdentity;
        createInfo.components.r = vk::ComponentSwizzle::eIdentity;
        createInfo.components.g = vk::ComponentSwizzle::eIdentity;
        createInfo.components.b = vk::ComponentSwizzle::eIdentity;
        createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        createInfo.subresourceRange.baseMipLevel = 0;
        createInfo.subresourceRange.levelCount = 1;
        createInfo.subresourceRange.baseArrayLayer = 0;
        createInfo.subresourceRange.layerCount = 1;
        vk::ResultValue<vk::ImageView> image_view_res = device.createImageView(createInfo);
        if (image_view_res.result != vk::Result::eSuccess)
            throw std::runtime_error("Failed to create image view");
        m_view = image_view_res.value;

        //            if (!frame.make_descriptor_resources(device, physical_Device))
        //                return std::unexpected(EmptyErr{});

         //           auto frame_descriptor_set_res = vkInit::allocate_descriptor_set(device, descriptorPool, descriptorSetLayout);
         //           if (!frame_descriptor_set_res)
         //               return std::unexpected(EmptyErr{});
        //            frame.descriptorSet = frame_descriptor_set_res.value();
    }

}
