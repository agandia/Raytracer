#pragma once

#include "glm/glm.hpp"

class Ray {
  public:
    Ray() {}
    Ray(const glm::dvec3& origin, const glm::dvec3& direction)
      : Ray(origin, direction, 0.0) {
    }
    Ray(const glm::dvec3& origin, const glm::dvec3& direction, double time)
      : origin_(origin), direction_(direction), time_(time) {
    }
  
    inline const glm::dvec3& origin() const { return origin_; }
    inline const glm::dvec3& direction() const { return direction_; }
    inline const double time() const { return time_; } 
  
    inline const glm::dvec3 at(double t) const {
      return origin_ + t * direction_;
    }
  
  private:
    glm::dvec3 origin_;
    glm::dvec3 direction_;
    double time_;
};