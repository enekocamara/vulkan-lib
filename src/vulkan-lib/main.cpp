#include <iostream>
#include <stdexcept>
#include "Config.h"
import <expected>;
import vulkan_lib.app;

//VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

int main()
{
    std::unique_ptr<vkl::App> app;
    try {
        app = std::make_unique<vkl::App>(1920, 1080);
    }catch (std::runtime_error &e){
        std::cerr << "CRITICAL ERROR, " << e.what() << '\n';
        return 1;
    }
    if (!app->run())
        return 1;
    return 0;
}

