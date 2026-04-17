module;
#include <vulkan-lib/Config.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <functional>
module vulkan_lib.Image;

import vulkan_lib.memory;
import debug_lib.result;
import vulkan_lib.commands;


namespace vkl {
	Image::Image(const Image::CreateInfo& info) :
		m_device(info.device),
		m_physical_device(info.physical_device),
		m_descriptor_layout(info.descriptor_layout),
		m_descriptor_pool(info.descriptor_pool),
		m_command_buffer(info.command_buffer),
		m_queue(info.queue)
	{
		m_pixels = stbi_load(
			info.file_path.string().c_str(),
			&m_dimmensions.x,
			&m_dimmensions.y,
			&m_channels,
			STBI_rgb_alpha
		);

		if (!m_pixels)
			throw std::runtime_error("Failed to load image");

		BufferInput buffer_input{
			.device = m_device,
			.physical_device = m_physical_device,
			.size = static_cast<std::size_t>(m_dimmensions.x * m_dimmensions.y * 4),
			.usage = vk::BufferUsageFlagBits::eTransferSrc,
			.properties = vk::MemoryPropertyFlagBits::eHostVisible |
				vk::MemoryPropertyFlagBits::eHostCoherent
		};
		auto buffer_res = create_buffer(buffer_input);
		if (!buffer_res)
			throw std::runtime_error("Failed to create buffer");

		auto mem_res = m_device.mapMemory(buffer_res.value().bufferMemory, 0, m_dimmensions.x * m_dimmensions.y * 4);
		if (mem_res.result != vk::Result::eSuccess)
			throw std::runtime_error("Failed to map memory");
		memcpy(mem_res.value, m_pixels, m_dimmensions.x * m_dimmensions.y * 4);
		m_device.unmapMemory(buffer_res.value().bufferMemory);

		vk::ImageCreateInfo image_info{
			{},
			vk::ImageType::e2D,
			vk::Format::eR8G8B8A8Srgb,
			vk::Extent3D{(uint32_t)m_dimmensions.x, (uint32_t)m_dimmensions.y, 1},
			1,//mipmap levels
			1,//array layers
			vk::SampleCountFlagBits::e1,
			vk::ImageTiling::eOptimal,
			vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled,
			vk::SharingMode::eExclusive,
			0, //queue family index count
			nullptr, //pqueue family indices
			vk::ImageLayout::eUndefined
		};
		auto image_res = m_device.createImage(image_info);

		if (image_res.result != vk::Result::eSuccess)
			throw std::runtime_error("Failed to create image");

		m_image = image_res.value;

		auto memory_requirements = m_device.getImageMemoryRequirements(m_image);
		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.allocationSize = memory_requirements.size;
		auto memory_type_index_res = find_memory_type_index(
			m_physical_device,
			memory_requirements.memoryTypeBits,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
		if (!memory_type_index_res)
			throw std::runtime_error("Failed to find memory type index");
		allocInfo.memoryTypeIndex = memory_type_index_res.value();

		m_device.allocateMemory(&allocInfo, nullptr, &m_image_memory);
		m_device.bindImageMemory(m_image, m_image_memory, 0);

		auto transition_img_1 = transition_image_layout(vk::ImageLayout::eTransferDstOptimal, info.command_pool, vk::PipelineStageFlagBits::eTopOfPipe, vk::PipelineStageFlagBits::eTransfer, vk::AccessFlagBits::eTransferWrite);
		if (!transition_img_1)
			throw std::runtime_error("Failed to transition image layout to eTransferDstOptimal");

		auto stc_res = single_time_command(m_device, info.command_pool, info.queue,
			[&](vk::CommandBuffer cmd) -> db::Result<db::EmptyOk>
			{
				vk::BufferImageCopy region{};

				region.bufferOffset = 0;
				region.bufferRowLength = 0;      // tightly packed
				region.bufferImageHeight = 0;    // tightly packed

				region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
				region.imageSubresource.mipLevel = 0;
				region.imageSubresource.baseArrayLayer = 0;
				region.imageSubresource.layerCount = 1;

				region.imageOffset = vk::Offset3D{ 0, 0, 0 };
				region.imageExtent = vk::Extent3D{
					(uint32_t)m_dimmensions.x,
					(uint32_t)m_dimmensions.y,
					1
				};
				cmd.copyBufferToImage(
					buffer_res.value().buffer,
					m_image,
					vk::ImageLayout::eTransferDstOptimal,
					1,
					&region
				);
				return db::EmptyOk{};
			});
		if (!stc_res)
			throw std::runtime_error("Failed to execute single time command");

		auto transition_img_2 = transition_image_layout(vk::ImageLayout::eShaderReadOnlyOptimal, info.command_pool, vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eFragmentShader, vk::AccessFlagBits::eShaderRead);
		if (!transition_img_2)
			throw std::runtime_error("Failed to transition image layout to eShaderReadOnlyOptimal");

		vk::SamplerCreateInfo sampler_info{};

		sampler_info.magFilter = vk::Filter::eLinear;
		sampler_info.minFilter = vk::Filter::eLinear;

		sampler_info.addressModeU = vk::SamplerAddressMode::eRepeat;
		sampler_info.addressModeV = vk::SamplerAddressMode::eRepeat;
		sampler_info.addressModeW = vk::SamplerAddressMode::eRepeat;

		sampler_info.anisotropyEnable = vk::False;
		sampler_info.maxAnisotropy = 1.0f;

		sampler_info.borderColor = vk::BorderColor::eIntOpaqueBlack;

		sampler_info.unnormalizedCoordinates = vk::False;

		sampler_info.compareEnable = vk::False;
		sampler_info.compareOp = vk::CompareOp::eAlways;

		sampler_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
		sampler_info.mipLodBias = 0.0f;
		sampler_info.minLod = 0.0f;
		sampler_info.maxLod = 0.0f;

		auto sampler_res = m_device.createSampler(sampler_info);
		if (sampler_res.result != vk::Result::eSuccess)
			throw std::runtime_error("Failed to create sampler");

		m_sampler = sampler_res.value;

		vk::ImageViewCreateInfo view_info{};

		view_info.image = m_image;
		view_info.viewType = vk::ImageViewType::e2D;
		view_info.format = vk::Format::eR8G8B8A8Srgb;

		view_info.components = vk::ComponentMapping{
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity,
			vk::ComponentSwizzle::eIdentity
		};

		view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		auto image_view_res = m_device.createImageView(view_info);
		if (image_view_res.result != vk::Result::eSuccess)
			throw std::runtime_error("Failed to create image view");

		m_image_view = image_view_res.value;
	}


	db::Result<db::EmptyOk> vkl::Image::transition_image_layout(vk::ImageLayout new_layout, vk::CommandPool command_pool, vk::PipelineStageFlags src_stage,
		vk::PipelineStageFlags dst_stage, vk::AccessFlagBits dst_access) {
		// Implementation goes here
		return single_time_command(m_device, command_pool, m_queue, [this, new_layout, src_stage, dst_stage, dst_access](vk::CommandBuffer command_buffer) -> db::Result<db::EmptyOk> {
			vk::ImageMemoryBarrier barrier{};

			barrier.oldLayout = m_image_layout;
			barrier.newLayout = new_layout;
			barrier.srcQueueFamilyIndex = vk::QueueFamilyIgnored;
			barrier.dstQueueFamilyIndex = vk::QueueFamilyIgnored;
			barrier.image = m_image;
			barrier.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			barrier.subresourceRange.baseMipLevel = 0;
			barrier.subresourceRange.levelCount = 1;
			barrier.subresourceRange.baseArrayLayer = 0;
			barrier.subresourceRange.layerCount = 1;
			barrier.srcAccessMask = m_access_mask; // TODO
			barrier.dstAccessMask = dst_access; // TODO
			//vk::PipelineStageFlags src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
			//vk::PipelineStageFlags dst_stage = vk::PipelineStageFlagBits::eTransfer;
			command_buffer.pipelineBarrier(
				src_stage,
				dst_stage,
				{},
				0, nullptr,
				0, nullptr,
				1, &barrier
			);
			return db::EmptyOk{};
			});
	}
}