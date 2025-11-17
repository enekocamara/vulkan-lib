module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.logging;

import <expected>;
import <iostream>;
import vulkan_lib.result;

namespace vkInit{
    export inline auto
    printDebug(const std::string& str) noexcept -> void{
        if constexpr(_DEBUG)
            std::cout << str << '\n';
    }
    export inline auto
    errprintDebug(const std::string& str) noexcept -> void{
        if constexpr(_DEBUG)
            std::cerr << "Error, " << str << '\n';
    }

    /*V
     * Debug call back:
     *
     *   typedef enum VkDebugUtilsMessageSeverityFlagBitsEXT {
     VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT = 0x00000001,
     VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT = 0x00000010,
     VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT = 0x00000100,
     VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT = 0x00001000,
     VK_DEBUG_UTILS_MESSAGE_SEVERITY_FLAG_BITS_MAX_ENUM_EXT = 0x7FFFFFFF
     } VkDebugUtilsMessageSeverityFlagBitsEXT;

     *   typedef enum VkDebugUtilsMessageTypeFlagBitsEXT {
     VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT = 0x00000001,
     VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT = 0x00000002,
     VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT = 0x00000004,
     VK_DEBUG_UTILS_MESSAGE_TYPE_FLAG_BITS_MAX_ENUM_EXT = 0x7FFFFFFF
     } VkDebugUtilsMessageTypeFlagBitsEXT;
     *   typedef struct VkDebugUtilsMessengerCallbackDataEXT {
     VkStructureType                              sType;
     const void*                                  pNext;
     VkDebugUtilsMessengerCallbackDataFlagsEXT    flags;
     const char*                                  pMessageIdName;
     int32_t                                      messageIdNumber;
     const char*                                  pMessage;
     uint32_t                                     queueLabelCount;
     const VkDebugUtilsLabelEXT*                  pQueueLabels;
     uint32_t                                     cmdBufLabelCount;
     const VkDebugUtilsLabelEXT*                  pCmdBufLabels;
     uint32_t                                     objectCount;
     const VkDebugUtilsObjectNameInfoEXT*         pObjects;
     } VkDebugUtilsMessengerCallbackDataEXT;

    */

    /*
       Logging callback function.

       \param messageSeverity describes the severity level of the message
       \param messageType describes the type of the message
       \param pCallbackData standard data associated with the message
       \param pUserData custom extra data which can be associated with the message
       \returns whether to end program execution
       */
    ///this is the debug call back function
    export inline VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
            void* pUserData
            ){
        std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
        return VK_FALSE;
    }

    /*
       Make a debug messenger

       \param instance The Vulkan instance which will be debugged.
       \param dldi dynamically loads instance based dispatch functions
       \returns the created messenger
    */

    export [[nodiscard]] inline auto
    make_debug_messanger(vk::Instance instance, vk::detail::DispatchLoaderDynamic dldi)noexcept -> std::expected<vk::DebugUtilsMessengerEXT, EmptyErr> {
        vk::DebugUtilsMessengerCreateInfoEXT createInfo = vk::DebugUtilsMessengerCreateInfoEXT(
                vk::DebugUtilsMessengerCreateFlagsEXT(),
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose | vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance,
                debugCallback,
                nullptr
                );
        vk::ResultValue<vk::DebugUtilsMessengerEXT> messengerR = instance.createDebugUtilsMessengerEXT(createInfo, nullptr, dldi);
        if (messengerR.result != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        return messengerR.value;
    }

    export [[nodiscard]] inline auto 
    log_transform_bits(vk::SurfaceTransformFlagsKHR bits) noexcept -> std::vector<std::string> {
        std::vector<std::string> lines;

        if (bits & vk::SurfaceTransformFlagBitsKHR::eIdentity)
            lines.push_back("identity");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate90)
            lines.push_back("rotate 90");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate180)
            lines.push_back("rotate 180");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eRotate270)
            lines.push_back("rotate 270");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirror)
            lines.push_back("horizontal mirror");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate90)
            lines.push_back("horizontal mirror 90");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate180)
            lines.push_back("horizontal mirror 180");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eHorizontalMirrorRotate270)
            lines.push_back("horizontal mirror 270");
        if (bits & vk::SurfaceTransformFlagBitsKHR::eInherit)
            lines.push_back("inherit");
                    
        return lines;
    }
    export [[nodiscard]] inline auto
    log_alpha_composite_bits(vk::CompositeAlphaFlagsKHR bits)noexcept -> std::vector<std::string> {
        std::vector<std::string> lines;

        if (bits & vk::CompositeAlphaFlagBitsKHR::eOpaque)
            lines.push_back("opaque");
        if (bits & vk::CompositeAlphaFlagBitsKHR::ePreMultiplied)
            lines.push_back("pre multiplied");
        if (bits & vk::CompositeAlphaFlagBitsKHR::ePostMultiplied)
            lines.push_back("post mutiplied");
        if (bits & vk::CompositeAlphaFlagBitsKHR::eInherit)
            lines.push_back("inherit");
        return lines;
    }
    
    export [[nodiscard]] inline auto
    log_image_usage_bits(vk::ImageUsageFlags bits)noexcept -> std::vector<std::string> {
        std::vector<std::string> lines;

        if (bits & vk::ImageUsageFlagBits::eTransferSrc) {
			lines.push_back("transfer src: image can be used as the source of a transfer command.");
		}
		if (bits & vk::ImageUsageFlagBits::eTransferDst) {
			lines.push_back("transfer dst: image can be used as the destination of a transfer command.");
		}
		if (bits & vk::ImageUsageFlagBits::eSampled) {
			lines.push_back("sampled: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot either of type VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE or \
VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, and be sampled by a shader.");
		}
		if (bits & vk::ImageUsageFlagBits::eStorage) {
			lines.push_back("storage: image can be used to create a VkImageView suitable for occupying a \
VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_STORAGE_IMAGE.");
		}
		if (bits & vk::ImageUsageFlagBits::eColorAttachment) {
			lines.push_back("color attachment: image can be used to create a VkImageView suitable for use as \
a color or resolve attachment in a VkFramebuffer.");
		}
		if (bits & vk::ImageUsageFlagBits::eDepthStencilAttachment) {
			lines.push_back("depth/stencil attachment: image can be used to create a VkImageView \
suitable for use as a depth/stencil or depth/stencil resolve attachment in a VkFramebuffer.");
		}
		if (bits & vk::ImageUsageFlagBits::eTransientAttachment) {
			lines.push_back("transient attachment: implementations may support using memory allocations \
with the VK_MEMORY_PROPERTY_LAZILY_ALLOCATED_BIT to back an image with this usage. This \
bit can be set for any image that can be used to create a VkImageView suitable for use as \
a color, resolve, depth/stencil, or input attachment.");
		}
		if (bits & vk::ImageUsageFlagBits::eInputAttachment) {
			lines.push_back("input attachment: image can be used to create a VkImageView suitable for \
occupying VkDescriptorSet slot of type VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT; be read from \
a shader as an input attachment; and be used as an input attachment in a framebuffer.");
		}
		if (bits & vk::ImageUsageFlagBits::eFragmentDensityMapEXT) {
			lines.push_back("fragment density map: image can be used to create a VkImageView suitable \
for use as a fragment density map image.");
		}
		if (bits & vk::ImageUsageFlagBits::eFragmentShadingRateAttachmentKHR) {
			lines.push_back("fragment shading rate attachment: image can be used to create a VkImageView \
suitable for use as a fragment shading rate attachment or shading rate image");
		}
        return lines;
    }

    export [[nodiscard]] inline auto
    log_present_mode(vk::PresentModeKHR presentMode) noexcept -> std::string {
		/*
		* // Provided by VK_KHR_surface
		typedef enum VkPresentModeKHR {
			VK_PRESENT_MODE_IMMEDIATE_KHR = 0,
			VK_PRESENT_MODE_MAILBOX_KHR = 1,
			VK_PRESENT_MODE_FIFO_KHR = 2,
			VK_PRESENT_MODE_FIFO_RELAXED_KHR = 3,
			// Provided by VK_KHR_shared_presentable_image
			VK_PRESENT_MODE_SHARED_DEMAND_REFRESH_KHR = 1000111000,
			// Provided by VK_KHR_shared_presentable_image
			VK_PRESENT_MODE_SHARED_CONTINUOUS_REFRESH_KHR = 1000111001,
		} VkPresentModeKHR;
		*/

		if (presentMode == vk::PresentModeKHR::eImmediate) {
			return "immediate: the presentation engine does not wait for a vertical blanking period \
to update the current image, meaning this mode may result in visible tearing. No internal \
queuing of presentation requests is needed, as the requests are applied immediately.";
		}
		if (presentMode == vk::PresentModeKHR::eMailbox) {
			return "mailbox: the presentation engine waits for the next vertical blanking period \
to update the current image. Tearing cannot be observed. An internal single-entry queue is \
used to hold pending presentation requests. If the queue is full when a new presentation \
request is received, the new request replaces the existing entry, and any images associated \
with the prior entry become available for re-use by the application. One request is removed \
from the queue and processed during each vertical blanking period in which the queue is non-empty.";
		}
		if (presentMode == vk::PresentModeKHR::eFifo) {
			return "fifo: the presentation engine waits for the next vertical blanking \
period to update the current image. Tearing cannot be observed. An internal queue is used to \
hold pending presentation requests. New requests are appended to the end of the queue, and one \
request is removed from the beginning of the queue and processed during each vertical blanking \
period in which the queue is non-empty. This is the only value of presentMode that is required \
to be supported.";
		}
		if (presentMode == vk::PresentModeKHR::eFifoRelaxed) {
			return "relaxed fifo: the presentation engine generally waits for the next vertical \
blanking period to update the current image. If a vertical blanking period has already passed \
since the last update of the current image then the presentation engine does not wait for \
another vertical blanking period for the update, meaning this mode may result in visible tearing \
in this case. This mode is useful for reducing visual stutter with an application that will \
mostly present a new image before the next vertical blanking period, but may occasionally be \
late, and present a new image just after the next vertical blanking period. An internal queue \
is used to hold pending presentation requests. New requests are appended to the end of the queue, \
and one request is removed from the beginning of the queue and processed during or after each \
vertical blanking period in which the queue is non-empty.";
		}
		if (presentMode == vk::PresentModeKHR::eSharedDemandRefresh) {
			return "shared demand refresh: the presentation engine and application have \
concurrent access to a single image, which is referred to as a shared presentable image. \
The presentation engine is only required to update the current image after a new presentation \
request is received. Therefore the application must make a presentation request whenever an \
update is required. However, the presentation engine may update the current image at any point, \
meaning this mode may result in visible tearing.";
		}
		if (presentMode == vk::PresentModeKHR::eSharedContinuousRefresh) {
			return "shared continuous refresh: the presentation engine and application have \
concurrent access to a single image, which is referred to as a shared presentable image. The \
presentation engine periodically updates the current image on its regular refresh cycle. The \
application is only required to make one initial presentation request, after which the \
presentation engine must update the current image without any need for further presentation \
requests. The application can indicate the image contents have been updated by making a \
presentation request, but this does not guarantee the timing of when it will be updated. \
This mode may result in visible tearing if rendering to the image is not timed correctly.";
		}
		return "none/undefined";
	}


}
