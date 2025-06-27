#pragma once

#include <glm/glm.hpp>

class OrthoNormalBasis {
public:
  OrthoNormalBasis(const glm::vec3& n) {
    axis[2] = glm::normalize(n);
    glm::dvec3 a = (glm::abs(axis[2].x) > 0.9) ? glm::dvec3(0, 1, 0) : glm::dvec3(1, 0, 0);
    axis[1] = glm::normalize(glm::cross(axis[2], a));
    axis[0] = glm::cross(axis[2], axis[1]);
  }

  const glm::dvec3& u() const { return axis[0]; }
  const glm::dvec3& v() const { return axis[1]; }
  const glm::dvec3& w() const { return axis[2]; }

  glm::dvec3 transform(const glm::dvec3& v) const {
    // Transform from basis coordinates to local space.
    return (v[0] * axis[0]) + (v[1] * axis[1]) + (v[2] * axis[2]);
  }

private:
  glm::dvec3 axis[3];
};