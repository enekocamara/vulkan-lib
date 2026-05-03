#include "QueueFamilies.hpp"
#include <vulkan-lib/Config.h>
#include <cstdint>
#include <vector>
#include <format>

#include "debug_lib/Logger.hpp"

namespace vkl {
	bool QueueFamilyIndices::is_complete() {
		return graphics_family.has_value() && present_family.has_value() && transfer_family.has_value() && compute_family.has_value();
	}
    
    auto find_queue_families(vk::PhysicalDevice device, vk::SurfaceKHR surface) noexcept -> QueueFamilyIndices {
        QueueFamilyIndices indices;
        
        std::vector<vk::QueueFamilyProperties> queue_families;
        queue_families = device.getQueueFamilyProperties();
       
        db::Logger::core_trace(std::format("Physical device supports {} queue families", queue_families.size()));
        uint32_t i = 0;
        //indices.transfer_family = 1;
        for (auto& queue_family : queue_families){
            if (queue_family.queueFlags & vk::QueueFlagBits::eCompute){
                indices.compute_family = i;
                db::Logger::core_trace(std::format("\tFamily queue index {} supports compute", i));
            }
            if (queue_family.queueFlags & vk::QueueFlagBits::eTransfer){
                indices.transfer_family = i;
                db::Logger::core_trace(std::format("\tFamily queue index {} supports transfer", i));
            }
            if (queue_family.queueFlags & vk::QueueFlagBits::eGraphics){
                indices.graphics_family = i;
                db::Logger::core_trace(std::format("\tFamily queue index {} supports graphics", i));
            }
            //bool support;
            auto result = device.getSurfaceSupportKHR(i, surface);

            if (result.result != vk::Result::eSuccess)
                continue;
            if (result.value){
                indices.present_family = i;
                db::Logger::core_trace(std::format("\tFamily queue index {} supports presenting", i));
            }
            if (indices.is_complete())
                return indices;
            i++;
        }
        return indices;
    }
}