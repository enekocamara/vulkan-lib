module;

#include <vulkan-lib/Config.h>
#include <cstdint>
#include <vector>
#include <format>

module vulkan_lib.queue_families;
import debug_lib.Logger;

namespace vkl {
	bool QueueFamilyIndices::is_complete() {
		return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
	}
    
    auto find_queue_families(vk::PhysicalDevice &device, vk::SurfaceKHR surface) noexcept -> QueueFamilyIndices {
        QueueFamilyIndices indices;
        
        std::vector<vk::QueueFamilyProperties> queueFamilies;
        queueFamilies = device.getQueueFamilyProperties();
       
        db::Logger::core_trace(std::format("Physical device supports {} queue families", queueFamilies.size()));
        uint32_t i = 0;
        indices.transferFamily = 1;
        for (auto& queueFamily : queueFamilies){
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics){
                indices.graphicsFamily = i;
                db::Logger::core_trace(std::format("\tFamily queue index {} supports graphics", i));
            }
            //bool support;
            auto result = device.getSurfaceSupportKHR(i, surface);

            if (result.result != vk::Result::eSuccess)
                continue;
            if (result.value){
                indices.presentFamily = i;
                db::Logger::core_trace(std::format("\tFamily queue index {} supports presenting", i));
            }
            if (indices.is_complete())
                return indices;
            i++;
        }
        return indices;
    }
}