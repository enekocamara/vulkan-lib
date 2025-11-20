export module vulkan_lib.app;

import <GLFW/glfw3.h>;
import <expected>;
import <memory>;
import vulkan_lib.engine;
import vulkan_lib.scene;
import vulkan_lib.result;

namespace vkl {
    export class App {
    public:
        App(int width, int height);
        ~App();
        App(const App& ref) = delete;
        App& operator=(const App& ref) = delete;
        [[nodiscard]] Result<EmptyOk> run();

    private:
        std::unique_ptr<Engine> graphicsEngine;
        Scene scene;
        GLFWwindow* window;

        double lastTime, currentTime;
        int numFrames;
        float frameTime;

        [[nodiscard]] Result<EmptyOk, Error> build_glfw_window(int width, int height)noexcept;
        void calculateFrameRate();

    };
}