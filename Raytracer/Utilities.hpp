#pragma once

#include "Constants.hpp"
#include "Interval.hpp"
#include <cstdlib>
#include <iostream>
#include <glm/glm.hpp> // vec3, dot, normalize, ...
#include <glm/gtx/norm.hpp> // length2


// Utility Functions
inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

inline double random_double() {
  // Returns a random real in [0, 1).
  return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
  // Returns a random real in [min, max).
  return min + (max - min) * random_double();
}

inline static glm::dvec3 random() {
  return glm::dvec3(random_double(), random_double(), random_double());
}

inline static glm::dvec3 random(double min, double max) {
  return glm::dvec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline glm::dvec3 random_unit_vector() {
  while (true) {
     glm::dvec3 p = random(-1, 1);
     double lengsq = glm::length2(p);
     if (1e-160 < lengsq && lengsq <= 1)
       return glm::normalize(p);
  }
}

inline glm::dvec3 random_on_hemisphere(const glm::dvec3& normal) {
  glm::dvec3 on_unit_sphere = random_unit_vector();
  if (glm::dot(on_unit_sphere, normal) > 0.0) { // In the same hemisphere as the normal
    return on_unit_sphere;
  } else {
    return -on_unit_sphere;
  }
}

inline void write_color(std::ostream& out, const glm::vec3 color) {
  // Translate the [0, 1] component values to the byte range [0, 255].
  static const Interval intensity(0.000, 0.999);
  
  out << static_cast<int>(256 * intensity.clamp(color.r)) << ' '
      << static_cast<int>(256 * intensity.clamp(color.g)) << ' '
      << static_cast<int>(256 * intensity.clamp(color.b)) << '\n';
}

