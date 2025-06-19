#pragma once

#include "glm/glm.hpp"

class Ray {
  public:
    Ray() {}
    Ray(const glm::dvec3& origin, const glm::dvec3& direction)
      : _origin(origin), _direction(direction) {
    }
  
    inline const glm::dvec3& origin() const { return _origin; }
    inline const glm::dvec3& direction() const { return _direction; }
  
    const glm::dvec3 at(double t) const {
      return _origin + t * _direction;
    }
  
  private:
    glm::dvec3 _origin;
    glm::dvec3 _direction;
};