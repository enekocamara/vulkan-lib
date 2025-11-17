module;

#include "vulkan-lib/Config.h"

export module vulkan_lib.camera3D;

import <chrono>;
import <glm/gtc/matrix_transform.hpp>;

namespace vkInit {

namespace player {
export enum Movement {
  Still = 0,
  Forward = 1,
  Backward = 2,
  Left = 4,
  Right = 8,
  Up = 16,
  Down = 32,
};
constexpr float mov_sp = 0.5f;          // m*s-1
constexpr float mov_sp_vertical = 0.2f; // m*s-1
} // namespace player

export struct Camera {

    glm::vec3 eye;
    glm::vec3 center;
    glm::vec3 up;
    int state;

  inline glm::mat4 getView() noexcept {
    return glm::lookAt(this->eye, this->center, this->up);
  }
  inline glm::mat4 getProjection(vk::Extent2D swapchainExtent) noexcept {
    glm::mat4 projection =
        glm::perspective(glm::radians(45.0f),
                         static_cast<float>(swapchainExtent.width) /
                             static_cast<float>(swapchainExtent.height),
                         0.1f, 10.f);
    projection[1][1] *= -1;
    return projection;
  }
  inline glm::mat4 getViewProjection(vk::Extent2D swapchainExtent) noexcept {
    return getProjection(swapchainExtent) * getView();
  }
  inline void update(std::chrono::duration<float> delta) {
    if (state == player::Movement::Still)
      return;
    float horizontal = float((((state & player::Movement::Right) >> 1) -
                              (state & player::Movement::Left)) >>
                             2);
    float vertical = float((((state & player::Movement::Up) << 1) -
                            (state & player::Movement::Down)) >>
                           5);
    float longitudinal = float((state & player::Movement::Forward) -
                               ((state & player::Movement::Backward) >> 1));
    glm::vec3 translate = (float)vertical * glm::vec3(0.0f, -1.0f, 0.0f) *
                              player::mov_sp * delta.count() +
                          longitudinal * glm::vec3(-1.0f, 0.0f, 0.0f) *
                              player::mov_sp * delta.count() +
                          horizontal * glm::vec3(0.0f, 0.0f, 1.0f) *
                              player::mov_sp * delta.count();
    eye += translate;
    center += translate;
  }
  inline void clearState() { this->state = player::Movement::Still; }
};
} // namespace vkInit
