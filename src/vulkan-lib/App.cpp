module;
#include <GLFW/glfw3.h>
#include <chrono>
module vulkan_lib.app;

import <sstream>;

import <iostream>;
import <format>;
import vulkan_lib.engine;
import vulkan_lib.scene;
import vulkan_lib.result;

//#include "Engine.h"
//#include "Scene.h"

App::App(int width, int height){
    auto res = build_glfw_window(width, height);
    if (!res)
        throw std::runtime_error(std::format("failed to make window: {}", res.error().message));
    graphicsEngine = std::make_unique<Engine>(width, height, window);
}

App::~App(){
}

[[nodiscard]] auto App::build_glfw_window(int width, int height)noexcept -> Result<EmptyOk, Error> {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

    window = glfwCreateWindow(width, height, "vulkan", nullptr, nullptr);
    if (window){
        if constexpr (_DEBUG)
            std::cout << "window successfully created.\n";
        return EmptyOk{};
    } else {
        if constexpr (_DEBUG)
            std::cout << "CRITICAL ERROR, failed to  build glfw window\n";
        return error("failed to build glfw window");
    }
}

[[nodiscard]] std::expected<EmptyOk, EmptyErr>    App::run(){
    std::chrono::time_point start = std::chrono::high_resolution_clock::now();
    while (!glfwWindowShouldClose(window)) {
        glfwPollEvents();
        std::chrono::time_point end = std::chrono::high_resolution_clock::now();//get end
        if (!graphicsEngine->render(scene, std::chrono::duration_cast<std::chrono::duration<float>>(end - start)))
            return std::unexpected(EmptyErr{});
        start = end;
        calculateFrameRate();
    }
    return EmptyOk{};
}

void App::calculateFrameRate(){
    currentTime = glfwGetTime();
    double delta = currentTime - lastTime;
    if (delta >= 1){
        int framerate {std::max(1, int(numFrames / delta))};
        std::stringstream title;
        title << "Running at " << framerate << " fps.";
        glfwSetWindowTitle(window, title.str().c_str());
        lastTime = currentTime;
        numFrames = -1;
        frameTime = float(1000.0 / framerate);
    }
    ++numFrames;
}
