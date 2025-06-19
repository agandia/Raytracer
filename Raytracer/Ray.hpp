#pragma once

#include "glm/glm.hpp"

class Ray {
  public:
    Ray() {}
    Ray(const glm::vec3& origin, const glm::vec3& direction)
      : origin(origin), direction(direction) {
    }

    const glm::vec3& origin() const { return origin; }
    const glm::vec3& direction() const { return direction; }

    const glm::vec3 at(double t) const {
        return origin + t * direction;
    }

  private:
    glm::vec3 origin;
    glm::vec3 direction;
}