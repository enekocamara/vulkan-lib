#pragma once
#include "vulkan-lib/Config.h"
#include "stb_image.h"
#include <filesystem>
#include <glm/glm.hpp>

#include "debug_lib/result.hpp"
#include "vulkan-lib/commands.hpp"
#include <variant>
#include <optional>
namespace fs = std::filesystem;
namespace vkl {
    class Image {
    public:
        struct Source{
            std::variant<fs::path, std::pair<size_t, void*>> source;
        };
        struct CreateInfo {
            vk::Device device;
            vk::PhysicalDevice physical_device;
            std::optional<Source> source;
			glm::uvec2 dimmensions;
            vk::Format format;
            vk::ImageUsageFlags usage;
            vk::ImageLayout layout;
            vk::AccessFlagBits access;
            vk::PipelineStageFlags pipeline_stage;
            vk::CommandBuffer command_buffer;
            vk::CommandPool command_pool;
            vk::Queue queue;
            vk::DescriptorSetLayout descriptor_layout;
            vk::DescriptorPool descriptor_pool;
        };

        Image() = delete;
        Image(const CreateInfo& info);
        ~Image() {};

        auto transition_image_layout(vk::ImageLayout new_layout, vk::CommandPool command_pool, vk::PipelineStageFlags src_stage, vk::PipelineStageFlags dst_stage, vk::AccessFlagBits dst_access) -> db::Result<db::EmptyOk>;
		auto get_image_view() const -> vk::ImageView { return m_image_view; }
		auto get_image() const -> vk::Image { return m_image; }
		auto get_descriptor_set() const -> vk::DescriptorSet { return m_descriptor_set; }
		auto get_sampler() const -> vk::Sampler { return m_sampler; }
    private:
        glm::ivec2 m_dimmensions;
        int m_channels;
        vk::Device m_device;
        vk::PhysicalDevice m_physical_device;

        vk::Image m_image;
        vk::DeviceMemory m_image_memory;
        vk::ImageView m_image_view;
        vk::Sampler m_sampler;
        vk::Format m_format;
        vk::ImageLayout m_image_layout = vk::ImageLayout::eUndefined;
        vk::AccessFlagBits m_access_mask = {};

        vk::DescriptorSetLayout m_descriptor_layout;
        vk::DescriptorSet m_descriptor_set;
        vk::DescriptorPool m_descriptor_pool;

        vk::CommandBuffer m_command_buffer;
        vk::Queue m_queue;
    };
}