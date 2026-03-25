export module vulkan_lib.app;

import <GLFW/glfw3.h>;
import <expected>;
import <memory>;
import vulkan_lib.scene;
import debug_lib.result;
namespace vkl {
    export class App {
    public:
        App(int width, int height);
        ~App();
        App(const App& ref) = delete;
        App& operator=(const App& ref) = delete;
        [[nodiscard]] db::Result<db::EmptyOk> run();

    private:
//        std::unique_ptr<Engine> graphicsEngine;
        Scene scene;
        GLFWwindow* window;

        double lastTime, currentTime;
        int numFrames;
        float frameTime;

        [[nodiscard]] db::Result<db::EmptyOk> build_glfw_window(int width, int height)noexcept;
        void calculateFrameRate();

    };
}