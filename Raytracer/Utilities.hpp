#pragma once

#include <glm/glm.hpp>
#include "Ray.hpp"

void write_color(std::ostream &out, const glm::vec3 color) {
    out << static_cast<int>(255.999f * color.r) << ' '
        << static_cast<int>(255.999f * color.g) << ' '
        << static_cast<int>(255.999f * color.b) << '\n';
}

bool hit_sphere(const glm::dvec3& center, double radius, const Ray& ray) {
  glm::dvec3 oc = center - ray.origin(); //ray from center of sphere to ray origin
  double a = glm::dot(ray.direction(), ray.direction());
  double b = 2.0 * glm::dot(oc, ray.direction());
  double c = glm::dot(oc, oc) - radius * radius;
  double discriminant = b * b - 4 * a * c;
  return (discriminant > 0);
}

glm::vec3 ray_color(const Ray& ray) {
  if(hit_sphere(glm::dvec3(0, 0, -1), 0.5, ray)) {
    return glm::vec3(1.0f, 0.0f, 0.0f); // Red color for the sphere
  }

  glm::dvec3 unit_direction = glm::normalize(ray.direction());
  float t = 0.5f * ((float)unit_direction.y + 1.0f); // Map y component to [0, 1]
  return (1.0f - t) * glm::vec3(1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f); // Gradient from white to blue
}
