module;
#include <vulkan-lib/Config.h>

module vulkan_lib.Swapchain;
import debug_lib.result;
import vulkan_lib.logging;
import <expected>;

namespace vkl {
    Swapchain::Swapchain(CreateInfo& info) {
        auto support_res = query_swapchain_support(info.physical_device, info.surface);
        if (!support_res)
            throw std::runtime_error("Failed to query swapchain support");
        SwapChainSupportDetails support = support_res.value();
        m_format = choose_swapchain_format(support.formats);
        m_extent = choose_swapchain_extent(info.width, info.height, support.capabilities);
        vk::PresentModeKHR presentMode = choose_swapchain_present_mode(support.presentModes);

        uint32_t imageCount = std::min(
            support.capabilities.maxImageCount,
            support.capabilities.minImageCount + 1
        );

        vk::SwapchainCreateInfoKHR createInfo{
            vk::SwapchainCreateFlagsKHR(),
            info.surface,
            imageCount,
            m_format.format,
            m_format.colorSpace,
            m_extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment
        };
        vkl::QueueFamilyIndices indices = vkl::find_queue_families(info.physical_device, info.surface);
        uint32_t queueFamilyIndices[] = { indices.graphicsFamily.value(), indices.presentFamily.value() };
        if (indices.graphicsFamily.value() != indices.presentFamily.value()) {
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform = support.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

        vk::ResultValue<vk::SwapchainKHR> swapchain_res = info.logical_device.createSwapchainKHR(createInfo, nullptr);
        if (swapchain_res.result != vk::Result::eSuccess)
            throw std::runtime_error("Failed create swapchain khr");
        m_swapchain = swapchain_res.value;

        vk::ResultValue<std::vector<vk::Image>> images_res = info.logical_device.getSwapchainImagesKHR(m_swapchain);
        if (images_res.result != vk::Result::eSuccess)
            throw std::runtime_error("Failed to get swapchain images khr");
        vk::CommandBufferAllocateInfo allocInfo{};
        allocInfo.commandPool = info.command_pool;
        allocInfo.level = vk::CommandBufferLevel::ePrimary;
        allocInfo.commandBufferCount = images_res.value.size();

        auto command_buffers_res = info.logical_device.allocateCommandBuffers(allocInfo);
        if (command_buffers_res.result != vk::Result::eSuccess)
            throw std::runtime_error("Failed to allocate command buffers");

        for (int i = 0; i < images_res.value.size(); i++) {
            m_frames.emplace_back(info.logical_device, info.physical_device, images_res.value[i], m_format.format, command_buffers_res.value[i]);
        }

        m_is_initialized = true;
    }
    auto Swapchain::query_swapchain_support(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface) noexcept -> db::Result<SwapChainSupportDetails> {
        SwapChainSupportDetails support;

        auto capabilities_res = physical_device.getSurfaceCapabilitiesKHR(surface);
        if (capabilities_res.result != vk::Result::eSuccess)
            return db::error("Failed to get surface capabilities");
        support.capabilities = capabilities_res.value;

        if constexpr (_DEBUG) {
            std::cout << "Swapchain can support the follow capabilities:\n";

            std::cout << "\t minimum image count: " << support.capabilities.minImageCount << '\n';
            std::cout << "\t maximum image count: " << support.capabilities.maxImageCount << '\n';

            std::cout << "\tcurrent extent: \n";
            std::cout << "\t\twidth: " << support.capabilities.currentExtent.width << '\n';
            std::cout << "\t\thegith: " << support.capabilities.currentExtent.height << '\n';

            std::cout << "\tmaximum extent allowed: \n";
            std::cout << "\t\twidth: " << support.capabilities.maxImageExtent.width << '\n';
            std::cout << "\t\thegith: " << support.capabilities.maxImageExtent.height << '\n';

            std::cout << "\tminimum extent allowed: \n";
            std::cout << "\t\twidth: " << support.capabilities.minImageExtent.width << '\n';
            std::cout << "\t\thegith: " << support.capabilities.minImageExtent.height << '\n';

            std::cout << "\tmaximum image array layers: " << support.capabilities.maxImageArrayLayers << '\n';

            std::cout << "\tsupported transformations:\n";
            std::vector<std::string> string_list = vkl::log_transform_bits(support.capabilities.supportedTransforms);
            for (auto line : string_list)
                std::cout << "\t\t" << line << '\n';

            std::cout << "\tcurrent transform: \n";
            string_list = vkl::log_transform_bits(support.capabilities.currentTransform);
            for (auto line : string_list)
                std::cout << "\t\t" << line << '\n';

            std::cout << "\tsupported alpha compositions: \n";
            string_list = vkl::log_alpha_composite_bits(support.capabilities.supportedCompositeAlpha);
            for (auto line : string_list)
                std::cout << "\t\t" << line << '\n';

            std::cout << "\tsupported image usage: \n";
            string_list = vkl::log_image_usage_bits(support.capabilities.supportedUsageFlags);
            for (auto line : string_list)
                std::cout << "\t\t" << line << '\n';
        }

        vk::ResultValue<std::vector<vk::SurfaceFormatKHR>> suface_formats_res = physical_device.getSurfaceFormatsKHR(surface);

        if (suface_formats_res.result != vk::Result::eSuccess)
            return db::error("Failed to get surface formats");
        support.formats = suface_formats_res.value;

        if constexpr (_DEBUG) {
            for (auto supportedFormat : support.formats) {
                std::cout << "supported pixel format: " << vk::to_string(supportedFormat.format) << '\n';
                std::cout << "supported color space: " << vk::to_string(supportedFormat.colorSpace) << '\n';
            }
        }

        vk::ResultValue<std::vector<vk::PresentModeKHR>> present_modes_res = physical_device.getSurfacePresentModesKHR(surface);
        if (present_modes_res.result != vk::Result::eSuccess)
            return db::error("Failed to get surface present modes");
        support.presentModes = present_modes_res.value;
        if constexpr (_DEBUG) {
            for (vk::PresentModeKHR presentMode : support.presentModes) {
                //std::cout << '\t' << log_present_mode(presentMode) << '\n'; TODO
            }
        }
        return support;

    }
    auto Swapchain::choose_swapchain_format(std::vector<vk::SurfaceFormatKHR> formats)noexcept -> vk::SurfaceFormatKHR {
        for (auto format : formats) {
            if (format.format == vk::Format::eB8G8R8A8Unorm
                && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear) {
                return format;
            }
        }
        return formats[0];
    }
    auto Swapchain::choose_swapchain_present_mode(std::vector<vk::PresentModeKHR> presentModes)noexcept -> vk::PresentModeKHR {
        for (auto presentMode : presentModes) {
            if (presentMode == vk::PresentModeKHR::eMailbox)
                return vk::PresentModeKHR::eMailbox;
        }
        return vk::PresentModeKHR::eFifo;
    }
    auto Swapchain::choose_swapchain_extent(int width, int height, vk::SurfaceCapabilitiesKHR capabilities) noexcept -> vk::Extent2D {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;
        vk::Extent2D extent = { (uint32_t)width, (uint32_t)height };
        extent.width = std::min(
            capabilities.maxImageExtent.width,
            std::max(capabilities.minImageExtent.width, extent.width));

        extent.width = std::min(
            capabilities.maxImageExtent.height,
            std::max(capabilities.minImageExtent.height, extent.height));
        return extent;
    }
    auto Swapchain::get_frames()->std::vector<vkl::SwapchainFrame>&{
        /*if constexpr (_DEBUG) {
            if (!m_not_initialized)
                
        }*/
        return m_frames;
        
    }
    auto get_swapchain() -> vk::SwapchainKHR;
    auto get_format() -> vk::SurfaceFormatKHR;
    auto get_extent() -> vk::Extent2D;
}