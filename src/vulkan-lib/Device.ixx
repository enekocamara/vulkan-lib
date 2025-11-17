module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.device;

import <expected>;
import <iostream>;
import vulkan_lib.logging;
import vulkan_lib.queueFamilies;
import vulkan_lib.result;

namespace vkInit{
       
    export [[nodiscard]] inline auto
    choose_physical_device(vk::Instance& instance) noexcept -> std::expected<vk::PhysicalDevice, EmptyErr> {
        if constexpr (_DEBUG)
            std::cout << "choosing physical device";
        vk::ResultValue<std::vector<vk::PhysicalDevice>> availableDevices = instance.enumeratePhysicalDevices();
        if (availableDevices.result != vk::Result::eSuccess){
            if constexpr (_DEBUG)
                std::cerr << "CRITICAL ERROR, no physical devices\n";
            return std::unexpected(EmptyErr{});
        } else {
            if constexpr (_DEBUG)
                std::cout << "There are " << availableDevices.value.size() << " physical device available.\n";
            return availableDevices.value[0];
        }
    }
   
    ///prints the family properties. does not check for _DEBUG
    export inline auto
    printQueueFamilieProperties(const vk::QueueFamilyProperties& properties)noexcept -> void{
       if (properties.queueFlags & vk::QueueFlagBits::eGraphics)
           std::cout << "\tSupports graphics.\n";
       if (properties.queueFlags & vk::QueueFlagBits::eCompute)
           std::cout << "\tSupports compute.\n";
       if (properties.queueFlags & vk::QueueFlagBits::eTransfer)
           std::cout << "\tSupports transfer.\n";
    }


    export [[nodiscard]] inline auto
    create_device(vk::PhysicalDevice physical_device, vk::SurfaceKHR surface) noexcept -> std::expected<vk::Device, EmptyErr> {
        vkUtil::QueueFamilyIndices indices = vkUtil::find_queue_families(physical_device, surface);
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
        if (_DEBUG)
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
            if constexpr (_DEBUG)
                std::cerr << "failed to create a logical device.\n";
            return std::unexpected(EmptyErr{});
        }
        return deviceV.value;
    }

    ///gets [graphics queue,present queue] from device, 
    export [[nodiscard]] inline auto
    get_queue(vk::PhysicalDevice physical_device, vk::Device device, vk::SurfaceKHR surface) noexcept -> vkUtil::QueueFamilyIndices{
        return  vkUtil::find_queue_families(physical_device, surface);
    }
}
