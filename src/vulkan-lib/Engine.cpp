module;

#include "vulkan-lib/Config.h"
#include <GLFW/glfw3.h>
module vulkan_lib.engine;

import <glm/gtc/matrix_transform.hpp>;
import <iostream>;
import <expected>;
import <stdexcept>; 
import vulkan_lib.framebuffer;
import vulkan_lib.instance;
import vulkan_lib.logging;
import vulkan_lib.pipeline;
import vulkan_lib.result;
import vulkan_lib.shader;
import vulkan_lib.swapchain;
import vulkan_lib.sync;
import vulkan_lib.commands;
import vulkan_lib.descriptors;
import vulkan_lib.device;
import vulkan_lib.renderStructs;


namespace vkl {

    /// engine default constructor, will throw std::runtime_error
    /// if it fails.
    Engine::Engine(int width, int height, GLFWwindow* window) : width(width), height(height), window(window) {
        if constexpr (_DEBUG)
            std::cout << "building engine.\n";
        auto instanceRes = make_instance();
        if (!instanceRes)
            throw std::runtime_error("failed to make instance");
        //(void)make_instance().map_err_throw("failed to make instance");
        if constexpr (_DEBUG) {
            dldi = vk::detail::DispatchLoaderDynamic(instance, vkGetInstanceProcAddr);
            auto debug_messenger_res = make_debug_messanger();
            if (!debug_messenger_res)
                throw std::runtime_error("failed to make debug messanger");
            debugMessenger = debug_messenger_res.value();
        }
        auto deviceRes = make_device();
        if (!deviceRes)
            throw std::runtime_error("failed to make device");

        //(void)make_device().map_err_throw("failed to make device");
        auto descriptor_set_layout_res = make_descriptor_set_layout();
        if (!descriptor_set_layout_res)
            throw std::runtime_error("failed to make descriptor set layout");
        //(void)make_descriptor_set_layout().map_err_throw(
        //        "failed to make descriptor set layout");
        auto pipeline_res = make_pipeline();
        if (!pipeline_res)
            throw std::runtime_error("failed to make pipeline");
        //(void)make_pipeline().map_err_throw("failed to make pipeline");
        //auto finalize_set_up_res = finalize_set_up();
        if (!finalize_set_up())
            throw std::runtime_error("failed to finalize setup");
        if (!make_assets())
            throw std::runtime_error("failed to make assets");
        //(void)make_assets().map_err_throw("failed to make assets");
        set_glfw_input_callback();
        init_camera();
    }

    Engine::~Engine() {
        if (device.waitIdle() != vk::Result::eSuccess)
            return;
        if constexpr (_DEBUG)
            std::cout << "deleting engine.\n";
        delete vertexManager;
        device.destroyPipeline(pipeline);
        device.destroyPipelineLayout(layout);
        device.destroyRenderPass(renderpass);
        cleanup_swapchain();
        device.destroyDescriptorSetLayout(descriptorSetLayout);
        device.destroyCommandPool(graphsPresCommandPool);
        device.destroyCommandPool(transferCommandPool);
        device.destroy();
        instance.destroySurfaceKHR(surface);
        if constexpr (_DEBUG)
            instance.destroyDebugUtilsMessengerEXT(debugMessenger, nullptr, dldi);
        instance.destroy();
        glfwTerminate();
    }
    void Engine::cleanup_swapchain() noexcept {
        for (auto& frame : swapchainFrames) {
            device.destroyFramebuffer(frame.framebuffer);
            device.destroyImageView(frame.view);
            device.destroyFence(frame.inFlightFence);
            device.destroySemaphore(frame.renderFinished);
            device.destroySemaphore(frame.imageAvailable);

            device.unmapMemory(frame.cameraDataBuffer.bufferMemory);
            device.freeMemory(frame.cameraDataBuffer.bufferMemory);
            device.destroyBuffer(frame.cameraDataBuffer.buffer);

            device.unmapMemory(frame.modelBuffer.bufferMemory);
            device.freeMemory(frame.modelBuffer.bufferMemory);
            device.destroyBuffer(frame.modelBuffer.buffer);
        }
        device.destroyDescriptorPool(descriptorPool);
        device.destroySwapchainKHR(swapchain);
    }

    void Engine::init_camera() noexcept {
        glm::vec3 eye = { 5.0f, 0.0f, -1.0f };
        glm::vec3 center = glm::vec3(0.0f);
        glm::vec3 up = { 0.0f, -1.0f, 0.0f };
        camera = { eye, center, up };
    }

    void Engine::engine_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (action == GLFW_PRESS) {
            if (GLFW_KEY_W == key)
                camera.state |= vkInit::player::Movement::Forward;
            if (GLFW_KEY_S == key)
                camera.state |= vkInit::player::Movement::Backward;
            if (GLFW_KEY_A == key)
                camera.state |= vkInit::player::Movement::Left;
            if (GLFW_KEY_D == key)
                camera.state |= vkInit::player::Movement::Right;
            if (GLFW_KEY_SPACE == key)
                camera.state |= vkInit::player::Movement::Up;
            if (GLFW_KEY_LEFT_SHIFT == key)
                camera.state |= vkInit::player::Movement::Down;
        }
        if (action == GLFW_RELEASE) {
            if (GLFW_KEY_W == key)
                camera.state &= (~(vkInit::player::Movement::Forward));
            if (GLFW_KEY_S == key)
                camera.state &= (~(vkInit::player::Movement::Backward));
            if (GLFW_KEY_A == key)
                camera.state &= (~(vkInit::player::Movement::Left));
            if (GLFW_KEY_D == key)
                camera.state &= (~(vkInit::player::Movement::Right));
            if (GLFW_KEY_SPACE == key)
                camera.state &= (~(vkInit::player::Movement::Up));
            if (GLFW_KEY_LEFT_SHIFT == key)
                camera.state &= (~(vkInit::player::Movement::Down));
        }
    }

    void wrapper_call_back(GLFWwindow* window, int key, int scancode, int action, int mods) {
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, true);
        else {
            Engine* engine = static_cast<Engine*>(glfwGetWindowUserPointer(window));
            engine->engine_callback(window, key, scancode, action, mods);
        }
    }

    void Engine::set_glfw_input_callback()noexcept {
        glfwSetWindowUserPointer(window, this);
        glfwSetKeyCallback(window, wrapper_call_back);
    }

    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::make_instance() noexcept {
        auto instance_res = vkInit::make_instance();
        if (!instance_res)
            return std::unexpected(EmptyErr{});
        instance = instance_res.value();

        VkSurfaceKHR c_stile_surface;
        if (glfwCreateWindowSurface(instance, window, nullptr, &c_stile_surface) !=
            VK_SUCCESS) {
            if constexpr (_DEBUG)
                std::cerr << "CRITICAL ERROR, failed to get window surface\n";
            return std::unexpected(EmptyErr{});
        }
        surface = c_stile_surface;
        return EmptyOk{};
    }

    [[nodiscard]] std::expected<vk::DebugUtilsMessengerEXT, EmptyErr>
        Engine::make_debug_messanger() noexcept {
        return vkInit::make_debug_messanger(instance, dldi);
        /*    Result<vk::DebugUtilsMessengerEXT, EmptyErr> debugMessangerR =
              vkInit::make_debug_messanger(instance, dldi); if (!debugMessangerR.is_ok())
              return err<vk::DebugUtilsMessengerEXT>();
              return ok(debugMessangerR.get_ok());*/
    }

    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::make_device() noexcept {
        auto phisical_device_res = vkInit::choose_physical_device(instance);
        if (!phisical_device_res)
            return std::unexpected(EmptyErr{});
        physicalDevice = phisical_device_res.value();
        auto device_res = vkInit::create_device(physicalDevice, surface);
        if (!device_res)
            return std::unexpected(EmptyErr{});
        device = device_res.value();
        vkUtil::QueueFamilyIndices indices =
            vkInit::get_queue(physicalDevice, device, surface);
        if (!indices.is_complete())
            return std::unexpected(EmptyErr{});
        graphicsQueue = { device.getQueue(indices.graphicsFamily.value(), 0),
            indices.graphicsFamily.value() };
        presentQueue = { device.getQueue(indices.presentFamily.value(), 0),
            indices.presentFamily.value() };
        transferQueue = { device.getQueue(indices.transferFamily.value(), 0),
            indices.transferFamily.value() };
        if (!vkInit::query_swapchain_support(physicalDevice, surface))
            return std::unexpected(EmptyErr{});
        if (!make_swapchain())
            return std::unexpected(EmptyErr{});

        frameNumber = 0;
        return EmptyOk{};
    }
    [[nodiscard]] std::expected<EmptyOk, EmptyErr>
        Engine::make_descriptor_set_layout() noexcept {
        vkInit::DescriptorSetLayoutData bindings = {};
        bindings.count = 2;

        bindings.indices.push_back(0);
        bindings.types.push_back(vk::DescriptorType::eUniformBuffer);
        bindings.counts.push_back(1);
        bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        bindings.indices.push_back(1);
        bindings.types.push_back(vk::DescriptorType::eStorageBuffer);
        bindings.counts.push_back(1);
        bindings.stages.push_back(vk::ShaderStageFlagBits::eVertex);

        auto descriptor_set_layout_res = vkInit::make_descriptor_set_layout(device, bindings);
        if (!descriptor_set_layout_res)
            return std::unexpected(EmptyErr{});
        descriptorSetLayout = descriptor_set_layout_res.value();
        return EmptyOk{};
    }

    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::make_swapchain() noexcept {

        auto bundle_res = vkInit::create_swapchain_bundle(device, physicalDevice, surface, width,
            height);
        if (!bundle_res)
            return std::unexpected(EmptyErr{});
        vkInit::SwapChainBundle bundle = bundle_res.value();
        /*Result<vkInit::SwapChainBundle, EmptyErr> bundleR =
          vkInit::create_swapchain_bundle(device, physicalDevice, surface, width,
          height); if (!bundleR.is_ok()) return err(); vkInit::SwapChainBundle bundle =
          bundleR.get_ok();*/
        swapchain = bundle.swapchain;
        swapchainExtent = bundle.extent;
        swapchainFrames = bundle.frames;
        swapchainFormat = bundle.format;
        maxFramesInFlight = static_cast<int>(swapchainFrames.size());
        return EmptyOk{};
    }
    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::recreate_swapchain() noexcept {
        width = 0;
        height = 0;
        while (width == 0 || height == 0) {
            glfwGetFramebufferSize(window, &width, &height);
            glfwPollEvents();
        }
        if (device.waitIdle() != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        cleanup_swapchain();

        if (!make_swapchain())
            return std::unexpected(EmptyErr{});
        if (!make_framebuffers())
            return std::unexpected(EmptyErr{});
        if (!make_frame_resources())
            return std::unexpected(EmptyErr{});
        vkInit::CommandBufferInputBundle commandBufferInput = {
            device, graphsPresCommandPool, swapchainFrames };

        auto main_command_buffer_res = vkInit::make_command_buffer(commandBufferInput);
        if (!main_command_buffer_res)
            return std::unexpected(EmptyErr{});
        mainCommandBuffer = main_command_buffer_res.value();
        if (!vkInit::make_frame_command_buffers(commandBufferInput))
            return std::unexpected(EmptyErr{});
        return EmptyOk{};
    }
    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::make_framebuffers() noexcept {
        vkInit::FramebufferInput framebufferInput;
        framebufferInput.device = device;
        framebufferInput.renderPass = renderpass;
        framebufferInput.swapchainExtent = swapchainExtent;
        return vkInit::make_framebuffers(framebufferInput, swapchainFrames);
    }
    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::make_frame_resources() noexcept {
        vkInit::DescriptorSetLayoutData bindings = {};
        bindings.count = 2;
        bindings.types.push_back(vk::DescriptorType::eUniformBuffer);
        bindings.types.push_back(vk::DescriptorType::eStorageBuffer);
        auto descriptor_pool_res = vkInit::make_descriptor_pool(device, static_cast<uint32_t>(swapchainFrames.size()), bindings);
        if (!descriptor_pool_res)
            return std::unexpected(EmptyErr{});
        descriptorPool = descriptor_pool_res.value();

        for (vkInit::SwapchainFrame& frame : swapchainFrames) {
            auto frame_in_flight_fence_res = vkInit::make_fence(device);
            auto frame_image_available_res = vkInit::make_semaphore(device);
            auto frame_render_finished_res = vkInit::make_semaphore(device);

            if (!frame_in_flight_fence_res || !frame_image_available_res || !frame_render_finished_res)
                return std::unexpected(EmptyErr{});
            frame.inFlightFence = frame_in_flight_fence_res.value();
            frame.imageAvailable = frame_image_available_res.value();
            frame.renderFinished = frame_render_finished_res.value();

            if (!frame.make_descriptor_resources(device, physicalDevice))
                return std::unexpected(EmptyErr{});
            auto frame_descriptor_set_res = vkInit::allocate_descriptor_set(device, descriptorPool, descriptorSetLayout);
            if (!frame_descriptor_set_res)
                return std::unexpected(EmptyErr{});
            frame.descriptorSet = frame_descriptor_set_res.value();
        }
        return EmptyOk{};
    }

    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::make_pipeline() noexcept {
        vkInit::GraphicsPipelineBundle specs = {};
        specs.device = device;
        specs.vertexFilepath = "vertex.spv";
        specs.fragmentFilepath = "fragment.spv";
        specs.extent = swapchainExtent;
        specs.swapchainImageFormat = swapchainFormat;
        specs.descriptorSetLayout = descriptorSetLayout;
        auto graphics_pipeline_res = vkInit::make_graphics_pipeline(specs);
        if (!graphics_pipeline_res)
            return std::unexpected(EmptyErr{});
        vkInit::GraphicsPipelineOutBundle graphics_pipeline = graphics_pipeline_res.value();

        pipeline = graphics_pipeline.pipeline;
        renderpass = graphics_pipeline.renderpass;
        layout = graphics_pipeline.layout;
        return {};
    }

    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::finalize_set_up() noexcept {
        if (!make_framebuffers())
            return std::unexpected(EmptyErr{});

        auto graphics_pres_command_pool_res = vkInit::make_command_pool(device, physicalDevice, surface,
            graphicsQueue.queueFamilyIndex);
        if (!graphics_pres_command_pool_res)
            return std::unexpected(EmptyErr{});
        graphsPresCommandPool = graphics_pres_command_pool_res.value();

        auto transfer_command_pool_res = vkInit::make_command_pool(device, physicalDevice, surface,
            transferQueue.queueFamilyIndex);
        if (!transfer_command_pool_res)
            return std::unexpected(EmptyErr{});
        transferCommandPool = transfer_command_pool_res.value();

        vkInit::CommandBufferInputBundle gpCommandPoolInput = {
            device, graphsPresCommandPool, swapchainFrames };
        auto main_command_buffer_res = vkInit::make_command_buffer(gpCommandPoolInput);
        if (!main_command_buffer_res)
            return std::unexpected(EmptyErr{});
        mainCommandBuffer = main_command_buffer_res.value();

        vkInit::CommandBufferInputBundle transCommandBufferInput = {
            device, transferCommandPool, swapchainFrames };
        auto transfer_command_buffer_res = vkInit::make_command_buffer(transCommandBufferInput);
        if (!transfer_command_buffer_res)
            return std::unexpected(EmptyErr{});

        transferCommandBuffer = transfer_command_buffer_res.value();

        if (!vkInit::make_frame_command_buffers(gpCommandPoolInput))
            return std::unexpected(EmptyErr{});
        if (!make_frame_resources())
            return std::unexpected(EmptyErr{});
        return EmptyOk{};
    }

    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::make_assets() noexcept {
        vertexManager = new VertexManager();
        std::vector<float> triangle_r = {
            0.0f, -0.05f, 1.0f, 0.0f, 0.0f,0.5f,0.0f,
            0.05f, 0.05f, 1.0f, 0.0f, 0.0f,1.0f,1.0f,
            -0.05f, 0.05f, 1.0f, 0.0f,  0.0f,0.0f,1.0f
        };
        /*std::vector<float> triangle_g = {
            0.0f, -0.05f, 0.0f,   1.0f,  0.0f, 0.05f, 0.05f, 0.0f,
            1.0f, 0.0f,   -0.05f, 0.05f, 0.0f, 1.0f,  0.0f,
        };
        std::vector<float> triangle_b = {
            0.0f, -0.05f, 0.0f, 0.0f,  1.0f,
            0.05f, 0.05f, 0.0f, 0.0f, 1.0f,
            -0.05f, 0.05f, 0.0f, 0.0f,  1.0f,
        };*/
        vertexManager->consume(MeshType::TRIANGLE_R, triangle_r);
        //vertexManager->consume(MeshType::TRIANGLE_G, triangle_g);
        //vertexManager->consume(MeshType::TRIANGLE_B, triangle_b);
        return vertexManager->finalize(device, physicalDevice, transferQueue.queue,
            transferCommandBuffer);

        //materials
        //std::unordered_map<MeshType, const char*>filenames = {
        //    {MeshType::TRIANGLE_R, "tex/"
        //};

    }


    void Engine::prepare_frame(uint32_t imageIndex, const Scene& scene) noexcept {
        vkInit::SwapchainFrame& _frame = swapchainFrames[imageIndex];
        /*glm::vec3 eye = {5.0f, 0.0f, -1.0f};
        glm::vec3 center = glm::vec3(0.0f);
        glm::vec3 up = {0.0f, -1.0f, 0.0f};
        glm::mat4 view = glm::lookAt(eye, center, up);
        glm::mat4 projection = glm::perspective(glm::radians(45.0f),
                static_cast<float>(swapchainExtent.width) / static_cast<float>(swapchainExtent.height),
                0.1f, 10.f);
        projection[1][1] *= -1;
        swapchainFrames[imageIndex].cameraData.view = view;
        swapchainFrames[imageIndex].cameraData.projection = projection;
        swapchainFrames[imageIndex].cameraData.viewProjection = projection * view;*/
        //swapchainFrames[imageIndex].cameraData.viewProjection = glm::mat4(1.0f);
        _frame.cameraData.viewProjection = camera.getViewProjection(swapchainExtent);

        //swapchainFrames[imageIndex].cameraData.viewProjection = glm::mat4(1.0f);
        memcpy(_frame.cameraDataWriteLocation,
            &(_frame.cameraData),
            sizeof(vkInit::UBO));

        size_t i = 0;
        _frame.modelTransforms[i++] = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0));

        /*
        for (const glm::vec3& position : scene.triangleRPositions){
            _frame.modelTransforms[i] = glm::translate(glm::mat4(1.0f),position);
            i++;
        }*/
        memcpy(_frame.modelBufferWriteLocation, _frame.modelTransforms.data(), i * sizeof(glm::mat4));
        _frame.write_descriptor_set(device);
    }


    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::prepare_scene(vk::CommandBuffer commandBuffer) noexcept {
        vk::Buffer vertexBuffers[] = { vertexManager->vertexBuffer.buffer };
        vk::DeviceSize offsets[] = { 0 };
        commandBuffer.bindVertexBuffers(0, 1, vertexBuffers, offsets);
        return EmptyOk{};
    }

    [[nodiscard]] std::expected<EmptyOk, EmptyErr> Engine::record_draw_buffer(vk::CommandBuffer commandBuffer, uint32_t imageIndex,
        const Scene& scene) noexcept {
        vk::CommandBufferBeginInfo beginInfo = {};
        if (commandBuffer.begin(beginInfo) != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});

        vk::RenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.renderPass = renderpass;
        renderPassInfo.framebuffer = swapchainFrames[imageIndex].framebuffer;
        renderPassInfo.renderArea.offset.x = 0;
        renderPassInfo.renderArea.offset.y = 0;
        renderPassInfo.renderArea.extent = swapchainExtent;
        vk::ClearValue clearColor = { std::array<float, 4>{0.0f, 0.5f, 0.25f, 1.0f} };
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;

        commandBuffer.beginRenderPass(&renderPassInfo, vk::SubpassContents::eInline);

        commandBuffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, layout, 0, swapchainFrames[imageIndex].descriptorSet, nullptr);

        commandBuffer.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);

        if (!prepare_scene(commandBuffer))
            return std::unexpected(EmptyErr{});

        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0, -1, 0));
        vkInit::ObjectData objectData;
        objectData.model = model;
        /* commandBuffer.pushConstants(layout, vk::ShaderStageFlagBits::eVertex, 0,
                 sizeof(objectData), &objectData);*/
        commandBuffer.draw(vertexManager->sizes[0], 3, vertexManager->offsets[0],
            0);

        commandBuffer.endRenderPass();

        if (commandBuffer.end() != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        return EmptyOk{};
    }
    std::expected<EmptyOk, EmptyErr> Engine::render(const Scene& scene, std::chrono::duration<float> delta) noexcept {
        if (device.waitForFences(1, &swapchainFrames[frameNumber].inFlightFence,
            VK_TRUE, std::numeric_limits<uint64_t>::max()) != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        vk::ResultValue<uint32_t> imageIndex = device.acquireNextImageKHR(
            swapchain, std::numeric_limits<uint64_t>::max(),
            swapchainFrames[frameNumber].imageAvailable, nullptr);
        if (imageIndex.result == vk::Result::eErrorOutOfDateKHR ||
            imageIndex.result == vk::Result::eSuboptimalKHR) {
            if (!recreate_swapchain()) {
                if constexpr (_DEBUG)
                    std::cerr << "CRITICAL ERROR failed to recreate swapchain\n";
                return std::unexpected(EmptyErr{});
            }
            return EmptyOk{};
        }
        else if (imageIndex.result != vk::Result::eSuccess) {
            if constexpr (_DEBUG)
                std::cerr << "failed to acquire the next image.\n";
            return std::unexpected(EmptyErr{});
        }

        camera.update(delta);

        vk::CommandBuffer commandBuffer = swapchainFrames[frameNumber].commandbuffer;

        if (commandBuffer.reset() != vk::Result::eSuccess);

        prepare_frame(imageIndex.value, scene);

        if (!record_draw_buffer(commandBuffer, imageIndex.value, scene)) {
            if constexpr (_DEBUG)
                std::cerr << "failed to draw the command buffer.\n";
            return std::unexpected(EmptyErr{});
        }
        vk::SubmitInfo submitInfo = {};
        vk::Semaphore waitSemaphores[] = {
            swapchainFrames[frameNumber].imageAvailable };
        vk::PipelineStageFlags waitStages[] = {
            vk::PipelineStageFlagBits::eColorAttachmentOutput };
        submitInfo.waitSemaphoreCount = 1;
        submitInfo.pWaitSemaphores = waitSemaphores;
        submitInfo.pWaitDstStageMask = waitStages;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        vk::Semaphore signalSemaphores[] = {
            swapchainFrames[frameNumber].renderFinished };
        submitInfo.signalSemaphoreCount = 1;
        submitInfo.pSignalSemaphores = signalSemaphores;
        if (device.resetFences(1, &swapchainFrames[frameNumber].inFlightFence) !=
            vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        if (graphicsQueue.queue.submit(submitInfo,
            swapchainFrames[frameNumber].inFlightFence) !=
            vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});

        vk::PresentInfoKHR presentInfo = {};
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = signalSemaphores;
        vk::SwapchainKHR swapchains[] = { swapchain };
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = swapchains;
        presentInfo.pImageIndices = &imageIndex.value;
        vk::Result resultPresent = presentQueue.queue.presentKHR(&presentInfo);
        if (resultPresent == vk::Result::eErrorOutOfDateKHR ||
            resultPresent == vk::Result::eSuboptimalKHR) {
            if (!recreate_swapchain()) {
                if constexpr (_DEBUG)
                    std::cerr << "CRITICAL ERROR failed to recreate swapchain\n";
                return std::unexpected(EmptyErr{});
            }
            return EmptyOk{};
        }
        if (resultPresent != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        frameNumber = (frameNumber + 1) % maxFramesInFlight;
        return EmptyOk{};
    }
}