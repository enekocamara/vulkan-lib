module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.swapchain;

//#include "Logging.h"
import <expected>;
import <iostream>;
import <string>;
import <vector>;

import vulkan_lib.swapchainFrame;
import vulkan_lib.result;
import vulkan_lib.logging;
import vulkan_lib.queueFamilies;
import vulkan_lib.swapchainFrame;

namespace vkInit{

    export struct SwapChainSupportDetails{
        vk::SurfaceCapabilitiesKHR capabilities;
        std::vector<vk::SurfaceFormatKHR> formats;
        std::vector<vk::PresentModeKHR> presentModes;
    };
    export struct SwapChainBundle{
        vk::SwapchainKHR swapchain;
        std::vector<SwapchainFrame>frames;
        vk::Format format;
        vk::Extent2D extent;
    };

    export [[nodiscard]] inline auto
    query_swapchain_support(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface) noexcept -> std::expected<SwapChainSupportDetails, EmptyErr> {
        SwapChainSupportDetails support;

        vk::ResultValue<vk::SurfaceCapabilitiesKHR> capabilitiesR = 
            physical_device.getSurfaceCapabilitiesKHR(surface);
        if (capabilitiesR.result != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        support.capabilities = capabilitiesR.value;

        if constexpr (_DEBUG){
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
            std::vector<std::string> stringList = vkInit::log_transform_bits(support.capabilities.supportedTransforms);
            for (auto line : stringList)
                std::cout << "\t\t" << line << '\n';

            std::cout << "\tcurrent transform: \n";
            stringList = vkInit::log_transform_bits(support.capabilities.currentTransform);
            for (auto line : stringList)
                std::cout << "\t\t" << line << '\n';

            std::cout << "\tsupported alpha compositions: \n";
            stringList = vkInit::log_alpha_composite_bits(support.capabilities.supportedCompositeAlpha);
            for (auto line : stringList)
                std::cout << "\t\t" << line << '\n';
            
            std::cout << "\tsupported image usage: \n";
            stringList = vkInit::log_image_usage_bits(support.capabilities.supportedUsageFlags);
            for (auto line : stringList)
                std::cout << "\t\t" << line << '\n';
        }
        
        vk::ResultValue<std::vector<vk::SurfaceFormatKHR>> sufaceFormatsR = physical_device.getSurfaceFormatsKHR(surface);
        
        if (sufaceFormatsR.result != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});;
        support.formats = sufaceFormatsR.value;
        
        if constexpr (_DEBUG){
            for (auto supportedFormat : support.formats) {
				std::cout << "supported pixel format: " << vk::to_string(supportedFormat.format) << '\n';
				std::cout << "supported color space: " << vk::to_string(supportedFormat.colorSpace) << '\n';
			}
        }

        vk::ResultValue<std::vector<vk::PresentModeKHR>> presentModesR = physical_device.getSurfacePresentModesKHR(surface);
        if (presentModesR.result != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        support.presentModes = presentModesR.value;
        if constexpr (_DEBUG){
    		for (vk::PresentModeKHR presentMode : support.presentModes) {
	    		std::cout << '\t' << log_present_mode(presentMode) << '\n';
	    	}
        }
        return support;
    }

    ///returns the format with formate B8G8R8A8Unorm and
    ///color space eSrgbNonlinear. return the first if none
    export [[nodiscard]] inline auto
    choose_swapchain_format(std::vector<vk::SurfaceFormatKHR> formats)noexcept -> vk::SurfaceFormatKHR {
        for (auto format : formats){
            if (format.format == vk::Format::eB8G8R8A8Unorm
                    && format.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear){
                return format;
            }
        }
        return formats[0];
    }
    
    ///returns mailbox if available, fifo if not
    export [[nodiscard]] inline auto
    choose_swapchain_present_mode(std::vector<vk::PresentModeKHR> presentModes)noexcept -> vk::PresentModeKHR {
        for (auto presentMode : presentModes){
            if (presentMode == vk::PresentModeKHR::eMailbox)
                return vk::PresentModeKHR::eMailbox;
        }
        return vk::PresentModeKHR::eFifo;
    }

    export [[nodiscard]] inline auto
    choose_swapchain_extent(uint32_t width, uint32_t height, vk::SurfaceCapabilitiesKHR capabilities)noexcept -> vk::Extent2D {
        if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
            return capabilities.currentExtent;
        vk::Extent2D extent = {width, height};
        extent.width = std::min(
                capabilities.maxImageExtent.width,
                std::max(capabilities.minImageExtent.width, width));

        extent.width = std::min(
                capabilities.maxImageExtent.height,
                std::max(capabilities.minImageExtent.height, width));
        return extent;
    }

    export [[nodiscard]] inline auto 
    create_swapchain_bundle(vk::Device logical_device, vk::PhysicalDevice physical_device, vk::SurfaceKHR surface, int width, int height) -> std::expected<SwapChainBundle, EmptyErr> {
        std::expected<SwapChainSupportDetails, EmptyErr> supportR = query_swapchain_support(physical_device,  surface);
        if (!supportR)
            return std::unexpected(EmptyErr{});
        SwapChainSupportDetails support = supportR.value();
        vk::SurfaceFormatKHR format = choose_swapchain_format(support.formats);
        vk::PresentModeKHR presentMode = choose_swapchain_present_mode(support.presentModes);
        vk::Extent2D extent = choose_swapchain_extent(width, height, support.capabilities);

        uint32_t imageCount = std::min(
            support.capabilities.maxImageCount,
            support.capabilities.minImageCount + 1
        );

        vk::SwapchainCreateInfoKHR createInfo(
            vk::SwapchainCreateFlagsKHR(),
            surface,
            imageCount,
            format.format,
            format.colorSpace,
            extent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment
                );
        vkUtil::QueueFamilyIndices indices = vkUtil::find_queue_families(physical_device, surface);
        uint32_t queueFamilyIndices[] = {indices.graphicsFamily.value(), indices.presentFamily.value()};
        if (indices.graphicsFamily.value() != indices.presentFamily.value()){
            createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }else {
            createInfo.imageSharingMode = vk::SharingMode::eExclusive;
        }

        createInfo.preTransform = support.capabilities.currentTransform;
        createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
        createInfo.presentMode  = presentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = vk::SwapchainKHR(nullptr);

        SwapChainBundle bundle{};
        vk::ResultValue<vk::SwapchainKHR> swapchainR = logical_device.createSwapchainKHR(createInfo, nullptr);
        if (swapchainR.result != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        bundle.swapchain = swapchainR.value;
        bundle.extent = extent;
        bundle.format = format.format;

        vk::ResultValue<std::vector<vk::Image>> imagesR = logical_device.getSwapchainImagesKHR(bundle.swapchain);
        if (imagesR.result != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        bundle.frames.resize(imagesR.value.size());
        for (uint32_t i = 0; i < bundle.frames.size(); i++){
            bundle.frames[i].image = imagesR.value[i];

            vk::ImageViewCreateInfo createInfo(
               vk::ImageViewCreateFlags(),
               bundle.frames[i].image,
               vk::ImageViewType::e2D,
               bundle.format
            );
            createInfo.components.a = vk::ComponentSwizzle::eIdentity;
            createInfo.components.r = vk::ComponentSwizzle::eIdentity;
            createInfo.components.g = vk::ComponentSwizzle::eIdentity;
            createInfo.components.b = vk::ComponentSwizzle::eIdentity;
            createInfo.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
            createInfo.subresourceRange.baseMipLevel = 0;
            createInfo.subresourceRange.levelCount = 1;
            createInfo.subresourceRange.baseArrayLayer = 0;
            createInfo.subresourceRange.layerCount = 1;
            vk::ResultValue<vk::ImageView> imageViewR = logical_device.createImageView(createInfo);
            if (imagesR.result != vk::Result::eSuccess)
                return std::unexpected(EmptyErr{});
            bundle.frames[i].view = imageViewR.value;
        }
        return bundle;
    }
}
