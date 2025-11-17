export module vulkan_lib.scene;

import <glm/glm.hpp>;

export enum class MeshType{
    TRIANGLE_R,
    TRIANGLE_G,
    TRIANGLE_B,
    NUM,
};

export class Scene{
    public:
        Scene();
        std::vector<glm::vec3>triangleRPositions;
        std::vector<glm::vec3>triangleGPositions;
        std::vector<glm::vec3>triangleBPositions;
};
