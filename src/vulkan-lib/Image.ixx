module;

#include "vulkan-lib/Config.h"
#include "stb_image.h"
#include <filesystem>
#include <glm/glm.hpp>

export module vulkan_lib.Image;

namespace fs = std::filesystem;
export namespace vkl {
    export class Image{
    public:
        struct CreateInfo {
            vk::Device device;
            vk::PhysicalDevice physical_device;
            fs::path file_path;
            vk::CommandBuffer command_buffer;
            vk::Queue queue;
            vk::DescriptorSetLayout descriptor_layout;
            vk::DescriptorPool descriptor_pool;
        };
        
        Image(const CreateInfo& info);
        ~Image() {};

        private:

            glm::ivec2 m_dimmensions;
            int m_channels;
            vk::Device m_device;
            vk::PhysicalDevice m_physical_device;
            fs::path m_filepath;
            stbi_uc *m_pixels;

            vk::Image m_image;
            vk::DeviceMemory m_image_memory;
            vk::ImageView m_image_view;
            vk::Sampler m_sampler;

            vk::DescriptorSetLayout m_descriptor_layout;
            vk::DescriptorSet m_descriptor_set;
            vk::DescriptorPool m_descriptor_pool;

            vk::CommandBuffer m_command_buffer;
            vk::Queue m_queue;
    };

}
