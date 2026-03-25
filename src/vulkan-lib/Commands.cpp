module;
#include <vulkan-lib/Config.h>

module vulkan_lib.commands;
import debug_lib.result;
import <expected>;
import <functional>;
namespace vkl {
	auto make_command_pool(vk::Device device, vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface, uint32_t queueFamilyIndex) -> db::Result<vk::CommandPool>{
		vk::CommandPoolCreateInfo poolInfo = {};
		poolInfo.flags = vk::CommandPoolCreateFlags() | vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
		poolInfo.queueFamilyIndex = queueFamilyIndex;

		vk::ResultValue<vk::CommandPool> commandPoolR = device.createCommandPool(poolInfo);
		if (commandPoolR.result != vk::Result::eSuccess)
			return db::error("Failed to create command pool");
		return commandPoolR.value;
	}
	auto  make_command_buffer(CommandBufferInputBundle inputBundle) -> db::Result<vk::CommandBuffer> {
		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = inputBundle.commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;

		vk::ResultValue<std::vector<vk::CommandBuffer>> commandBufferR = inputBundle.device.allocateCommandBuffers(allocInfo);
		if (commandBufferR.result != vk::Result::eSuccess)
			return db::error("Failed to allocate command buffers");
		return commandBufferR.value[0];
	}
	auto make_frame_command_buffers(CommandBufferInputBundle inputBundle) -> db::Result<db::EmptyOk> {
		vk::CommandBufferAllocateInfo allocInfo = {};
		allocInfo.commandPool = inputBundle.commandPool;
		allocInfo.level = vk::CommandBufferLevel::ePrimary;
		allocInfo.commandBufferCount = 1;
		for (int i = 0; i < inputBundle.frames.size(); i++) {
			vk::ResultValue<std::vector<vk::CommandBuffer>> commandBufferR = inputBundle.device.allocateCommandBuffers(allocInfo);
			if (commandBufferR.result != vk::Result::eSuccess)
				return db::error("Failed to allocate command buffers");
			inputBundle.frames[i].m_command_buffer = commandBufferR.value[0];
		}
		return db::EmptyOk{};
	}


	
	auto single_time_command(vk::Device device, vk::CommandPool command_pool, std::function<std::expected<db::EmptyOk, db::Error>()> lambda) -> db::Result<db::EmptyOk> {
		auto begin_res = begin_single_time_command(device, command_pool);
		if (!begin_res)
			return db::error("Failed to begin single time command", std::move(begin_res.error()));
	}

	
	auto begin_single_time_command(vk::Device device, vk::CommandPool command_pool) -> std::expected<vk::CommandBuffer, db::Error> {
		vk::CommandBufferAllocateInfo alloc_info{};
		alloc_info.commandPool = command_pool;
		alloc_info.level = vk::CommandBufferLevel::ePrimary;
		alloc_info.commandBufferCount = 1;

		auto command_buffer_res = device.allocateCommandBuffers(alloc_info);
		if (command_buffer_res.result != vk::Result::eSuccess)
			return db::error("Failed to allocate command buffer");


		vk::CommandBufferBeginInfo begin_info{};
		begin_info.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;

		command_buffer_res.value[0].begin(begin_info);

		return command_buffer_res.value[0];
	}


	auto end_single_time_command(vk::Device device, vk::CommandPool command_pool, vk::CommandBuffer command_buffer, vk::Queue queue) -> db::Result<db::EmptyOk> {
		command_buffer.end();

		vk::SubmitInfo submit_info{};
		submit_info.commandBufferCount = 1;
		submit_info.pCommandBuffers = &command_buffer;

		if (queue.submit(submit_info) != vk::Result::eSuccess)
			return db::error("Failed to submit buffer to queue");
		if (queue.waitIdle() != vk::Result::eSuccess)
			return db::error("Failed to wait idle");

		device.freeCommandBuffers(command_pool, command_buffer);
		return db::EmptyOk{};
	}
}
