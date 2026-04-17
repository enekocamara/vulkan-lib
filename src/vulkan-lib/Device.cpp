module;

#include <vulkan-lib/Config.h>

module vulkan_lib.device;

import debug_lib.result;
import vulkan_lib.queue_families;
import debug_lib.Logger;
namespace vkl {

    auto choose_physical_device(vk::Instance& instance) noexcept -> db::Result<vk::PhysicalDevice> {
        db::Logger::core_trace("Chossing physical device");
        vk::ResultValue<std::vector<vk::PhysicalDevice>> availableDevices = instance.enumeratePhysicalDevices();
        if (availableDevices.result != vk::Result::eSuccess){
            db::Logger::core_error("CRITICAL ERROR, no physical devices");
            return db::error("No physical devices available");
        } else {
            db::Logger::core_trace(std::format("There are {} physical device available",availableDevices.value.size()));
            return availableDevices.value[0];
        }
    }
    auto print_queue_family_properties(const vk::QueueFamilyProperties& properties)noexcept -> void{
        if (properties.queueFlags & vk::QueueFlagBits::eGraphics)
            db::Logger::core_trace("\tSupports graphics.");
        if (properties.queueFlags & vk::QueueFlagBits::eCompute)
            db::Logger::core_trace("\tSupports compute.");
        if (properties.queueFlags & vk::QueueFlagBits::eTransfer)
            db::Logger::core_trace("\tSupports transfer.");
    }


    auto create_device(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface) noexcept -> db::Result<vk::Device> {
        vkl::QueueFamilyIndices indices = vkl::find_queue_families(physical_device, surface);
        std::vector<uint32_t> unique_indices;
        unique_indices.push_back(indices.presentFamily.value());
        if (indices.presentFamily.value() != indices.graphicsFamily.value())
            unique_indices.push_back(indices.graphicsFamily.value());
        unique_indices.push_back(indices.transferFamily.value());
        float queuePriority = 1.f;
        std::vector<vk::DeviceQueueCreateInfo> queueCreateInfo;
        for (uint32_t queueFamilyIndex : unique_indices){
            queueCreateInfo.emplace_back(vk::DeviceQueueCreateFlags(),
                queueFamilyIndex,
                1, &queuePriority
            );
        }
        std::vector<const char *>deviceExtensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME
        };

        vk::PhysicalDeviceFeatures features;

        features.samplerAnisotropy = true;

        std::vector<const char*>layers;
        if constexpr (_DEBUG)
            layers.push_back("VK_LAYER_KHRONOS_validation");

        vk::DeviceCreateInfo createInfo(
            vk::DeviceCreateFlags(),
            static_cast<uint32_t>(queueCreateInfo.size()), queueCreateInfo.data(),
            static_cast<uint32_t>(layers.size()),layers.data(),
            static_cast<uint32_t>(deviceExtensions.size()),deviceExtensions.data(),
            &features
            );
        vk::ResultValue<vk::Device> deviceV = physical_device.createDevice(createInfo);
        if (deviceV.result != vk::Result::eSuccess){
            db::Logger::core_error("Failed to create a logical device");
            return db::error("Failed to create a logical device");
        }
        return deviceV.value;
    }
    auto get_queue(vk::PhysicalDevice physical_device, vk::Device device, vk::SurfaceKHR surface) noexcept -> vkl::QueueFamilyIndices{
        return  vkl::find_queue_families(physical_device, surface);
    }
}