module;

#include "vulkan-lib/Config.h"
#include "stb_image.h"
#include <filesystem>
#include <glm/glm.hpp>

export module vulkan_lib.Image;
import debug_lib.result;
import vulkan_lib.commands;


namespace fs = std::filesystem;
namespace vkl {
    export class Image {
    public:
        struct CreateInfo {
            vk::Device device;
            vk::PhysicalDevice physical_device;
            fs::path file_path;
			//glm::uvec2 dimmensions;
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
        fs::path m_filepath;
        stbi_uc* m_pixels;

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