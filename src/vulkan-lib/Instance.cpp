module;
#include <GLFW/glfw3.h>
#include <vulkan-lib/Config.h>
#include <vector>
module vulkan_lib.instance;
import debug_lib.result;
import debug_lib.Logger;


namespace vkl {

    auto supported(std::vector<const char*>& extensions, std::vector<const char*>& layers) -> bool {
        vk::ResultValue<std::vector<vk::ExtensionProperties>> supportedExtensionsV = vk::enumerateInstanceExtensionProperties();
        if (supportedExtensionsV.result != vk::Result::eSuccess)
            return false;
        if constexpr (_DEBUG) {
            db::Logger::core_trace("Device can support the following extensions: ");
            for (auto supportedExtension : supportedExtensionsV.value)
                db::Logger::core_trace(std::format("\t\"{}\"", supportedExtension.extensionName.data()));
            db::Logger::core_trace("\n");
        }

        bool found;
        for (auto extension : extensions) {
            found = false;
            for (auto supportedExtension : supportedExtensionsV.value) {
                if (strcmp(extension, supportedExtension.extensionName) == 0) {
                    found = true;
                    db::Logger::core_trace(std::format("\tExtension \"{}\" is supported", extension));
                    break;
                }
            }
            if (!found) {
                db::Logger::core_error(std::format("\tExtension \"{}\" is not supported", extension));
                return false;
            }
        }

        db::Logger::core_trace("\n");

        vk::ResultValue<std::vector<vk::LayerProperties>> supportedLayersV = vk::enumerateInstanceLayerProperties();
        if (supportedLayersV.result != vk::Result::eSuccess)
            return false;
        if constexpr (_DEBUG) {
            db::Logger::core_trace("Device can support the following layers: ");
            for (auto supportedLayer : supportedLayersV.value)
                db::Logger::core_trace(std::format("\t\" {}\"", supportedLayer.layerName.data()));
            db::Logger::core_trace("\n");
        }

        found = false;
        for (auto layer : layers) {
            found = false;
            for (auto supportedLayers : supportedLayersV.value) {
                if (strcmp(layer, supportedLayers.layerName) == 0) {
                    found = true;
                    db::Logger::core_trace(std::format("\tLayer \"{}\" is supported", layer));
                    break;
                }
            }
            if (!found) {
                db::Logger::core_trace(std::format("\tLayer \"{}\" is not supported", layer));
                return false;
            }
        }
        return true;
    }

    auto make_instance(const std::string& app_name, const std::string& engine_name) -> db::Result<vk::Instance> {
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
