module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.instance;

import <GLFW/glfw3.h>;
import <vector>;
import <iostream>;
import <expected>;
import <format>;
import debug_lib.result;
import debug_lib.Logger;


namespace vkInit{

    export [[nodiscard]] inline auto
    supported(std::vector<const char *>& extensions, std::vector<const char *>&layers) -> bool{
        vk::ResultValue<std::vector<vk::ExtensionProperties>> supportedExtensionsV = vk::enumerateInstanceExtensionProperties();
        if (supportedExtensionsV.result != vk::Result::eSuccess)
            return false;
        if constexpr (_DEBUG){
            db::Logger::core_error("Device can support the following extensions: ");
            for (auto supportedExtension : supportedExtensionsV.value)
                db::Logger::core_error(std::format("\t\"{}\"", supportedExtension.extensionName));
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
    make_instance(const std::string& app_name, const std::string& engine_name) -> db::Result<vk::Instance> {
        db::Logger::core_trace("Making instance");

        uint32_t version = 0;
        vkEnumerateInstanceVersion(&version);

        db::Logger::core_trace(std::format("System can support vulkan Variant: {}, Major: {}, Minor: {}, Patch: {}",
            VK_API_VERSION_VARIANT(version),
            VK_API_VERSION_MAJOR(version),
            VK_API_VERSION_MINOR(version),
            VK_API_VERSION_PATCH(version)
            ));

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

        if constexpr (_DEBUG){
            extensions.push_back("VK_EXT_debug_utils");
            
            db::Logger::core_trace("Extensions required:");
            for (auto extension : extensions)
                db::Logger::core_trace(std::format("\t\"{}\"", extension));
        }
        
        std::vector<const char *> layers;


        if constexpr (_DEBUG)
            layers.push_back("VK_LAYER_KHRONOS_validation");

        if (!supported(extensions, layers))
            return db::error("VK_LAYER_KHRONOS_validation not supported");

        vk::InstanceCreateInfo createInfo(
                vk::InstanceCreateFlags(),
                &appInfo,
                static_cast<uint32_t>(layers.size()),layers.data(),
                static_cast<uint32_t>(extensions.size()),extensions.data()
                );

        vk::ResultValue<vk::Instance> instance = vk::createInstance(
                createInfo, nullptr);
        if (instance.result != vk::Result::eSuccess)
            return db::error("Failed to creaet instance");
        else 
            return instance.value;
    }
}
