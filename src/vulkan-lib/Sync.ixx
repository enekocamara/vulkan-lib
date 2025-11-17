module;
#include "Config.h"
export module vulkan_lib.sync;


import vulkan_lib.result;
import <expected>;
import <iostream>;

namespace vkInit{
    export [[nodiscard]] inline auto
    make_semaphore(vk::Device device) noexcept -> std::expected<vk::Semaphore, EmptyErr> {
        vk::SemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.flags = vk::SemaphoreCreateFlags();

        vk::ResultValue<vk::Semaphore> semaphoreR = device.createSemaphore(semaphoreInfo);
        if (semaphoreR.result != vk::Result::eSuccess){
            if constexpr (_DEBUG)
                std::cerr << "Failed to create semaphore\n";
            return std::unexpected(EmptyErr{});
        }
        return semaphoreR.value;
    }

    export [[nodiscard]] inline auto
     make_fence(vk::Device device) noexcept -> std::expected<vk::Fence, EmptyErr> {
        vk::FenceCreateInfo fenceInfo= {};
        fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

        vk::ResultValue<vk::Fence> fenceR = device.createFence(fenceInfo);
        if (fenceR.result != vk::Result::eSuccess){
            if constexpr (_DEBUG)
                std::cerr << "Failed to create fence\n";
            return std::unexpected(EmptyErr{});
        }
        return fenceR.value;
    }
}

