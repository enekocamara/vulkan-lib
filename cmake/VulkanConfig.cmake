set(VULKAN_SDK "C:/VulkanSDK/1.3.275.0" CACHE PATH "Path to Vulkan SDK")

# Help CMake find the package config inside the SDK
list(APPEND CMAKE_PREFIX_PATH "${VULKAN_SDK}")

# Now find Vulkan normally
find_package(Vulkan REQUIRED)

message(STATUS "Using Vulkan SDK at: ${VULKAN_SDK}")