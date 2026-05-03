#pragma once
#include <glm/glm.hpp>
#include <vector>

enum class MeshType{
    TRIANGLE_R,
    TRIANGLE_G,
    TRIANGLE_B,
    NUM,
};

class Scene{
    public:
        Scene();
        std::vector<glm::vec3>triangleRPositions;
        std::vector<glm::vec3>triangleGPositions;
        std::vector<glm::vec3>triangleBPositions;
};
