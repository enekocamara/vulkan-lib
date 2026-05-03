#pragma once

#include "vulkan-lib/Config.h"
#include <vector>
#include <span>
#include <functional>


#include "debug_lib/result.hpp"
#include "vulkan-lib/QueueFamilies.hpp"
#include "vulkan-lib/SwapchainFrame.hpp"

namespace vkl{
    struct SwapChainSupportDetails {
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };
    class Swapchain {
    public:
        struct CreateInfo {
            vk::Device logical_device;
            vk::PhysicalDevice physical_device;
            vk::SurfaceKHR surface;
            int width;
            int height;
            vk::CommandPool command_pool;
            vk::DescriptorPool descriptor_pool;
            std::span<vk::DescriptorSetLayout> descriptor_set_layouts;
            std::function<db::Result<db::EmptyOk>(SwapchainFrame& swapchainFrame)> populate_descriptor_sets;
            //vk::DescriptorSetLayout fragment_descriptor_set_layout;
            //vk::DescriptorSetLayout vertex_descriptor_set_layout;
        };
        
        Swapchain(std::nullptr_t null) {}
        Swapchain(CreateInfo& info);
        static auto choose_swapchain_format(std::vector<vk::SurfaceFormatKHR> formats)noexcept -> vk::SurfaceFormatKHR;
        static auto choose_swapchain_present_mode(std::vector<vk::PresentModeKHR> presentModes)noexcept -> vk::PresentModeKHR;
        static auto choose_swapchain_extent(int height, int width, vk::SurfaceCapabilitiesKHR capabilities)noexcept -> vk::Extent2D;
        static auto query_swapchain_support(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface) noexcept -> db::Result<SwapChainSupportDetails>;
        auto get_frames()->std::vector<vkl::SwapchainFrame>&;
        auto get_format() -> vk::SurfaceFormatKHR { return m_format; }
        auto get_extent() -> vk::Extent2D { return m_extent; }
        auto get_swapchain() -> vk::SwapchainKHR {
            return m_swapchain;
        };
   private:
        
        vk::SwapchainKHR m_swapchain;
        std::vector<SwapchainFrame> m_frames;
        vk::SurfaceFormatKHR m_format;
        vk::Extent2D m_extent;
        uint32_t m_frames_in_flight;
        bool m_is_initialized = false;
    };
    
}
