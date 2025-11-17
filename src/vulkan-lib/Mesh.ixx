module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.mesh;

namespace vkMesh {
    export [[nodiscard]] inline auto
    getPosColorBindingDescription() noexcept -> vk::VertexInputBindingDescription {
        vk::VertexInputBindingDescription bindingDescription = {};
        bindingDescription.binding = 0;
        bindingDescription.stride = 7 * sizeof(float);
        bindingDescription.inputRate = vk::VertexInputRate::eVertex;

        return bindingDescription;
    }

    export [[nodiscard]] inline auto
    getPosColorAttributeDescriptions()noexcept -> std::vector<vk::VertexInputAttributeDescription> {
        std::vector<vk::VertexInputAttributeDescription> attributes;
        attributes.resize(3);

        attributes[0].binding = 0;
        attributes[0].location = 0;
        attributes[0].format = vk::Format::eR32G32Sfloat;
        attributes[0].offset = 0;

        attributes[1].binding = 0;
        attributes[1].location = 1;
        attributes[1].format = vk::Format::eR32G32B32Sfloat;
        attributes[1].offset = 2 * sizeof(float);

        attributes[2].binding = 0;
        attributes[2].location = 2;
        attributes[2].format = vk::Format::eR32G32Sfloat;
        attributes[2].offset = 5 * sizeof(float);
        return attributes;
    }
}
