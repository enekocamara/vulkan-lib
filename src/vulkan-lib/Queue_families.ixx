module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.queueFamilies;


import <optional>;
import <iostream>;

export namespace vkUtil {

    ///stores the queue family indices. option none if there were
    ///no suitable queue families
    export struct QueueFamilyIndices{
        std::optional<uint32_t> graphicsFamily;
        std::optional<uint32_t> presentFamily;
        std::optional<uint32_t> transferFamily;

        ///check if both are presenst
        bool is_complete() {
            return graphicsFamily.has_value() && presentFamily.has_value() && transferFamily.has_value();
        }
    };

    export struct Queue{
        vk::Queue queue;
        uint32_t queueFamilyIndex;
    };

    export [[nodiscard]] inline auto
    find_queue_families(vk::PhysicalDevice &device, vk::SurfaceKHR surface) noexcept -> QueueFamilyIndices {
        QueueFamilyIndices indices;
        
        std::vector<vk::QueueFamilyProperties> queueFamilies;
        queueFamilies = device.getQueueFamilyProperties();
       
        if (_DEBUG)
            std::cout << "Physical device supports " << queueFamilies.size() << " queue families.\n";
        uint32_t i = 0;
        indices.transferFamily = 1;
        for (auto queueFamily : queueFamilies){
            if (queueFamily.queueFlags & vk::QueueFlagBits::eGraphics){
                indices.graphicsFamily = i;
                if constexpr (_DEBUG){
                    std::cout << "family queue index " << i << " supports graphics\n";
                }
            }
            //bool support;
            auto result = device.getSurfaceSupportKHR(i, surface);

            if (result.result != vk::Result::eSuccess)
                continue;
            if (result.value){
                indices.presentFamily = i;
                if constexpr(_DEBUG)
                    std::cout << "queue family " << i << "supports presenting.\n";
            }
            if (indices.is_complete())
                return indices;
            i++;
        }
        return indices;
    }
}
