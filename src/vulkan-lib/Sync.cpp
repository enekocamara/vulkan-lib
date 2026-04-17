module;
#include <vulkan-lib/Config.h>
module vulkan_lib.sync;
import debug_lib.result;

namespace vkl {

    auto make_semaphore(vk::Device device) noexcept -> db::Result<vk::Semaphore>{
        vk::SemaphoreCreateInfo semaphoreInfo = {};
        semaphoreInfo.flags = vk::SemaphoreCreateFlags();

        vk::ResultValue<vk::Semaphore> semaphoreR = device.createSemaphore(semaphoreInfo);
        if (semaphoreR.result != vk::Result::eSuccess) {
            return db::error("Failed to create semaphore");
        }
        return semaphoreR.value;
    }
    auto make_fence(vk::Device device) noexcept -> db::Result<vk::Fence> {
        vk::FenceCreateInfo fenceInfo= {};
        fenceInfo.flags = vk::FenceCreateFlags() | vk::FenceCreateFlagBits::eSignaled;

        vk::ResultValue<vk::Fence> fenceR = device.createFence(fenceInfo);
        if (fenceR.result != vk::Result::eSuccess){
            return db::error("Failed to create fence");
        }
        return fenceR.value;
    }
}
