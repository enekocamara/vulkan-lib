module;
#include <vulkan-lib/Config.h>;
#include "stb_image.h"

module vulkan_lib.Image;

import vulkan_lib.memory;
import <exception>;


namespace vkl {
	Image::Image(const Image::CreateInfo& info):
		m_device(info.device),
		m_physical_device(info.physical_device),
		m_descriptor_layout(info.descriptor_layout),
		m_descriptor_pool(info.descriptor_pool),
		m_command_buffer(info.command_buffer),
		m_queue(info.queue)
	{
		m_pixels = stbi_load(
			"texture.png",
			&m_dimmensions.x,
			&m_dimmensions.y,
			&m_channels,
			STBI_rgb_alpha
		);

		BufferInput buffer_input{
			.device = m_device,
			.physical_device = m_physical_device,
			.size = static_cast<std::size_t>(m_dimmensions.x * m_dimmensions.y * 4),
			.usage = vk::BufferUsageFlagBits::eTransferSrc,
			.properties = vk::MemoryPropertyFlagBits::eHostVisible|
				vk::MemoryPropertyFlagBits::eHostCoherent
		};
		auto buffer_res = create_buffer(buffer_input);
		if (!buffer_res)
			throw std::runtime_error("Failed to create buffer");
		allocate_buffer_memory(buffer_res.value(), buffer_input);

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
		auto result = m_device.createImage(image_info);

		if (result.result != vk::Result::eSuccess)
			throw std::runtime_error("Failed to create image");

		m_image = result.value;

		auto memory_requirements = m_device.getImageMemoryRequirements(m_image);
		vk::MemoryAllocateInfo allocInfo{};
		allocInfo.allocationSize = memory_requirements.size;
		auto memory_type_index_res = vkl::findMemoryTypeIndex(
			m_physical_device,
			memory_requirements.memoryTypeBits,
			vk::MemoryPropertyFlagBits::eDeviceLocal
		);
		if (!memory_type_index_res)
			throw std::runtime_error("Failed to find memory type index");
		allocInfo.memoryTypeIndex = memory_type_index_res.value();

		m_device.allocateMemory(&allocInfo, nullptr, &m_image_memory);
		m_device.bindImageMemory(m_image, m_image_memory, 0);
		/*vk::TransitionImageLayout(image,
			format,
			vk::ImageLayout::eUndefined,
			vk::ImageLayout::eTransferDstOptimal);
			*/
	}
}
