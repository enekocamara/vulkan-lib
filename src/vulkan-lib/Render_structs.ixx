module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.renderStructs;

import <glm/glm.hpp>;
import <glm/gtc/matrix_transform.hpp>;

namespace vkInit{
    export struct ObjectData{
        glm::mat4 model;
    };

}
