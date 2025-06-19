#pragma once

#include <cmath>
#include <iostream>
#include <limits>
#include <glm/glm.hpp> // vec3, dot, normalize, ...
#include "Ray.hpp"
#include "Hittable.hpp"

// Constants
const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

void write_color(std::ostream& out, const glm::vec3 color) {
    out << static_cast<int>(255.999f * color.r) << ' '
        << static_cast<int>(255.999f * color.g) << ' '
        << static_cast<int>(255.999f * color.b) << '\n';
}

glm::vec3 ray_color(const Ray& ray, const Hittable& world) {
  HitRecord hit_record;
  if (world.hit(ray, 0, infinity, hit_record)) {
    return 0.5f * (glm::vec3(hit_record.normal) + glm::vec3(1.0f));
  }

  glm::dvec3 unit_direction = glm::normalize(ray.direction());
  float t = 0.5f * ((float)unit_direction.y + 1.0f); // Map y component to [0, 1]
  return (1.0f - t) * glm::vec3(1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f); // Gradient from white to blue for the background
}
