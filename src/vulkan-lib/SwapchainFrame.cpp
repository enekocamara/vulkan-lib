#include "SwapchainFrame.hpp"
#include "vulkan-lib/Config.h"
#include <glm/glm.hpp>


#include "debug_lib/result.hpp"
#include "vulkan-lib/memory.hpp"
#include "vulkan-lib/logging.hpp"
#include "vulkan-lib/sync.hpp"

namespace vkl {
    SwapchainFrame::SwapchainFrame(const SwapchainFrame::CreateInfo& info) :
        m_image(info.image),
        m_command_buffer(info.command_buffer),
        m_descriptor_sets(info.descriptor_sets)
    {
        auto frame_in_flight_fence_res = vkl::make_fence(info.device);
        auto frame_image_available_res = vkl::make_semaphore(info.device);
        auto frame_render_finished_res = vkl::make_semaphore(info.device);

        if (!frame_in_flight_fence_res || !frame_image_available_res || !frame_render_finished_res)
            throw std::runtime_error("Failed to creaet sync objects");
        m_in_flight_fence = frame_in_flight_fence_res.value();
        m_image_available = frame_image_available_res.value();
        m_render_finished = frame_render_finished_res.value();


        vk::ImageViewCreateInfo image_view_info(
            vk::ImageViewCreateFlags(),
            m_image,
            vk::ImageViewType::e2D,
            info.format
        );
        image_view_info.components.a = vk::ComponentSwizzle::eIdentity;
        image_view_info.components.r = vk::ComponentSwizzle::eIdentity;
        image_view_info.components.g = vk::ComponentSwizzle::eIdentity;
        image_view_info.components.b = vk::ComponentSwizzle::eIdentity;
        image_view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.levelCount = 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.layerCount = 1;
        vk::ResultValue<vk::ImageView> image_view_res = info.device.createImageView(image_view_info);
        if (image_view_res.result != vk::Result::eSuccess)
            throw std::runtime_error("Failed to create image view");
        m_view = image_view_res.value;

        vk::ImageCreateInfo depth_image_info{};
        depth_image_info.imageType = vk::ImageType::e2D;
        depth_image_info.format = vk::Format::eD32Sfloat;
        depth_image_info.extent = vk::Extent3D{info.width, info.height, 1};
        depth_image_info.mipLevels = 1;
        depth_image_info.arrayLayers = 1;
        depth_image_info.samples = vk::SampleCountFlagBits::e1;
        depth_image_info.tiling = vk::ImageTiling::eOptimal;
        depth_image_info.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
        depth_image_info.sharingMode = vk::SharingMode::eExclusive;
        depth_image_info.initialLayout = vk::ImageLayout::eUndefined;

        auto depth_image_res = info.device.createImage(depth_image_info);
        if (depth_image_res.result != vk::Result::eSuccess)
            throw std::runtime_error("Faied to create depth buffer image");
        m_depth_image = depth_image_res.value;
        
        //depth image memory

        vk::MemoryAllocateInfo depth_alloc_info{};
        vk::MemoryRequirements depth_mem_req = info.device.getImageMemoryRequirements(m_depth_image);
        depth_alloc_info.allocationSize = depth_mem_req.size;
    	auto memory_type_index_res = find_memory_type_index(
			info.physical_device,
			depth_mem_req.memoryTypeBits,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
		if (!memory_type_index_res)
			throw std::runtime_error("Failed to find memory type index");
		depth_alloc_info.memoryTypeIndex = memory_type_index_res.value();


        auto depth_memory_res = info.device.allocateMemory(depth_alloc_info);
        if (depth_memory_res.result != vk::Result::eSuccess)
            throw std::runtime_error("Failed to allocate memory for depth buffer");
        
        if (info.device.bindImageMemory(m_depth_image, depth_memory_res.value, 0) != vk::Result::eSuccess)
            throw std::runtime_error("Failed to bind depth image memory");

        //depth image view
        vk::ImageViewCreateInfo depth_view_info{};
        depth_view_info.image = m_depth_image;
        depth_view_info.viewType = vk::ImageViewType::e2D;
        depth_view_info.format = vk::Format::eD32Sfloat;

        depth_view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eDepth;
        depth_view_info.subresourceRange.baseMipLevel = 0;
        depth_view_info.subresourceRange.levelCount = 1;
        depth_view_info.subresourceRange.baseArrayLayer = 0;
        depth_view_info.subresourceRange.layerCount = 1;
        
        auto depth_view_res = info.device.createImageView(depth_view_info);

        if (depth_view_res.result != vk::Result::eSuccess)
            throw std::runtime_error("Failed to create depth image view");
        m_depth_view = depth_view_res.value;

        //            if (!frame.make_descriptor_resources(device, physical_Device))
        //                return std::unexpected(EmptyErr{});

         //           auto frame_descriptor_set_res = vkInit::allocate_descriptor_set(device, descriptorPool, descriptorSetLayout);
         //           if (!frame_descriptor_set_res)
         //               return std::unexpected(EmptyErr{});
        //            frame.descriptorSet = frame_descriptor_set_res.value();
    }

}
