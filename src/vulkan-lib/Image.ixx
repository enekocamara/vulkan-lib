module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.image;

import "stb_image.h";

namespace vkImage {
    export struct TextureInputBundle{
        vk::Device device;
        vk::PhysicalDevice physicalDevice;
        const char * filename;
        vk::CommandBuffer commandBuffer;
        vk::Queue queue;
        vk::DescriptorSetLayout layout;
        vk::DescriptorPool descriptorPool;
    };
    
    export class Texture{
        
        private:
            int width,height, channels;
            vk::Device device;
            vk::PhysicalDevice physicalDevice;
            const char *filename;
            stbi_uc *pixels;

            vk::Image image;
            vk::DeviceMemory imageMemory;
            vk::ImageView imageView;
            vk::Sampler sampler;

            vk::DescriptorSetLayout layout;
            vk::DescriptorSet descriptorSet;
            vk::DescriptorPool descriptorPool;

            vk::CommandBuffer cmdBuffer;
            vk::Queue queue;
    };

}
