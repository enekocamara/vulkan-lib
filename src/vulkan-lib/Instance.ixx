module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.instance;

import <GLFW/glfw3.h>;
import <vector>;
import <iostream>;
import <expected>;
import vulkan_lib.result;


namespace vkInit{

    export [[nodiscard]] inline auto
    supported(std::vector<const char *>& extensions, std::vector<const char *>&layers) -> bool{
        vk::ResultValue<std::vector<vk::ExtensionProperties>> supportedExtensionsV = vk::enumerateInstanceExtensionProperties();
        if (supportedExtensionsV.result != vk::Result::eSuccess)
            return false;
        if constexpr (_DEBUG){
            std::cout << "Device can support the following extensions: \n";
            for (auto supportedExtension : supportedExtensionsV.value)
                std::cout << "\t\"" << supportedExtension.extensionName << "\"\n";
            std::cout << '\n';
        }

        bool found;
        for (auto extension : extensions){
            found = false;
            for (auto supportedExtension : supportedExtensionsV.value){
                if (strcmp(extension, supportedExtension.extensionName) == 0){
                    found = true;
                    if constexpr (_DEBUG)
                        std::cout << "\tExtension \"" << extension << "\" is supported\n";
                    break;
                }
            }
            if (!found){
                if constexpr(_DEBUG)
                    std::cerr << "\tExtension \"" << extension << "\" is not supported\n";
                return false;
            }
        }
        
        if constexpr (_DEBUG)
            std::cout << '\n';

        vk::ResultValue<std::vector<vk::LayerProperties>> supportedLayersV = vk::enumerateInstanceLayerProperties();
        if (supportedLayersV.result != vk::Result::eSuccess)
            return false;
        if constexpr (_DEBUG){
            std::cout << "Device can support the following layers: \n";
            for (auto supportedLayer : supportedLayersV.value)
                std::cout << "\t\"" << supportedLayer.layerName << "\"\n";
            std::cout << '\n';
        }


        found = false;
        for (auto layer : layers){
            found = false;
            for (auto supportedLayers : supportedLayersV.value){
                if (strcmp(layer, supportedLayers.layerName) == 0){
                    found = true;
                    if constexpr (_DEBUG)
                        std::cout << "\tLayer \"" << layer<< "\" is supported\n";
                    break;
                }
            }
            if (!found){
                if constexpr(_DEBUG)
                    std::cerr << "\tLayer \"" << layer << "\" is not supported\n";
                return false;
            }
        }


        return true;
    }

    export [[nodiscard]] inline auto
    make_instance() -> std::expected<vk::Instance, EmptyErr> {
        if constexpr (_DEBUG)
            std::cout << "making instance...\n";

        uint32_t version = 0;
        vkEnumerateInstanceVersion(&version);
        if constexpr (_DEBUG){
            std::cout << "System can support vulkan Variant: " << VK_API_VERSION_VARIANT(version)
                << ",  Major: " << VK_API_VERSION_MAJOR(version)
                << ",  Minor: " << VK_API_VERSION_MINOR(version)
                << ",  Patch: " << VK_API_VERSION_PATCH(version) << '\n';
        }


        vk::ApplicationInfo appInfo(
                "name", // app name
                version,
                "engine", //  engine name
                version,
                version
                );

        uint32_t glfwExtensionCount = 0;
        const char ** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

        std::vector<const char *>extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

        if constexpr (_DEBUG)
            extensions.push_back("VK_EXT_debug_utils");
            
        if constexpr (_DEBUG){
            std::cout << "extensions required:\n";
            for (auto extension : extensions){
                std::cout << "\t\""<< extension << "\"\n";
            }
        }
        
        std::vector<const char *> layers;

        if constexpr (_DEBUG)
            layers.push_back("VK_LAYER_KHRONOS_validation");

        if (!supported(extensions, layers))
            return std::unexpected(EmptyErr{});

        vk::InstanceCreateInfo createInfo(
                vk::InstanceCreateFlags(),
                &appInfo,
                static_cast<uint32_t>(layers.size()),layers.data(),
                static_cast<uint32_t>(extensions.size()),extensions.data()
                );

        vk::ResultValue<vk::Instance> instance = vk::createInstance(
                createInfo, nullptr);
        if (instance.result != vk::Result::eSuccess)
            return std::unexpected(EmptyErr{});
        else 
            return instance.value;
        
    }
}
