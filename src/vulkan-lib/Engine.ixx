///This file contains the declarations of the engine class
module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.engine;

import <GLFW/glfw3.h>;
import <unordered_map>;
import <expected>;
import <chrono>;

import vulkan_lib.swapchainFrame;
import vulkan_lib.vertexManager;
import vulkan_lib.queueFamilies;
import vulkan_lib.camera3D;
import vulkan_lib.scene;
import vulkan_lib.image;
import vulkan_lib.result;

///my custom engine class
namespace vkl {
    export class Engine
    {
    public:
        Engine(int width, int height, GLFWwindow* window);
        ~Engine();
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> render(const Scene& scene, std::chrono::duration<float> delta) noexcept;
        void engine_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
    private:
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> make_instance() noexcept;
        [[nodiscard]] std::expected<vk::DebugUtilsMessengerEXT, EmptyErr> make_debug_messanger() noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> make_device() noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> make_swapchain() noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> recreate_swapchain() noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> make_descriptor_set_layout() noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> make_pipeline() noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> finalize_set_up() noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> make_framebuffers() noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> make_frame_resources() noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> record_draw_buffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex, const Scene& scene) noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> make_assets() noexcept;
        [[nodiscard]] std::expected<EmptyOk, EmptyErr> prepare_scene(vk::CommandBuffer commandBuffer) noexcept;

        void set_glfw_input_callback()noexcept;
        void prepare_frame(uint32_t imageIndex, const Scene& scene) noexcept;

        void init_camera()noexcept;

        void cleanup_swapchain() noexcept;

        int width;
        int height;
        GLFWwindow* window;
        //instance related
        vk::Instance instance;
        vk::DebugUtilsMessengerEXT debugMessenger{ nullptr };
        vk::detail::DispatchLoaderDynamic dldi;
        vk::SurfaceKHR surface{ nullptr };

        //device related
        vk::PhysicalDevice physicalDevice{ nullptr };
        vk::Device device{ nullptr };
        vkUtil::Queue graphicsQueue{ nullptr };
        vkUtil::Queue presentQueue{ nullptr };
        vkUtil::Queue transferQueue{ nullptr };

        //swapchain related
        vk::SwapchainKHR swapchain;
        std::vector<vkInit::SwapchainFrame>swapchainFrames;
        vk::Format swapchainFormat;
        vk::Extent2D swapchainExtent;

        //pipeline related variables
        vk::Pipeline pipeline;
        vk::RenderPass renderpass;
        vk::PipelineLayout layout;

        //commands
        vk::CommandPool graphsPresCommandPool;
        vk::CommandPool transferCommandPool;
        vk::CommandBuffer mainCommandBuffer;
        vk::CommandBuffer transferCommandBuffer;

        //sync objects
        int maxFramesInFlight, frameNumber;

        //descriptor objects
        vk::DescriptorSetLayout descriptorSetLayout;
        vk::DescriptorPool descriptorPool;
        //assets
        VertexManager* vertexManager;
        std::unordered_map<MeshType, Image*> materials;

        vkInit::Camera camera;
    };

}
