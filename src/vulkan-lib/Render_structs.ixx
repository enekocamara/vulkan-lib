module;

#include "vulkan-lib/Config.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

export module vulkan_lib.renderStructs;


namespace vkInit{
    export struct ObjectData{
        glm::mat4 model;
    };

}
