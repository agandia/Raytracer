#pragma once

#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include "Ray.hpp"

void write_color(std::ostream &out, const glm::vec3 color) {
    out << static_cast<int>(255.999f * color.r) << ' '
        << static_cast<int>(255.999f * color.g) << ' '
        << static_cast<int>(255.999f * color.b) << '\n';
}

double hit_sphere(const glm::dvec3& center, double radius, const Ray& ray) {
  glm::dvec3 oc = center - ray.origin(); //ray from center of sphere to ray origin
  double a = glm::length2(ray.direction());
  double h = glm::dot(ray.direction(), oc);
  double c = glm::length2(oc) - radius * radius;
  double discriminant = h * h - a * c;
  
  if(discriminant < 0) {
    return -1.0; // No intersection
  } else {
    return (h - glm::sqrt(discriminant)) / a; // Return the nearest intersection point
  }
}

glm::vec3 ray_color(const Ray& ray) {
  const double hit_test = hit_sphere(glm::dvec3(0, 0, -1), 0.5, ray);
  if(hit_test > 0.0) {
    glm::vec3 normal = glm::normalize(ray.at(hit_test) - glm::dvec3(0, 0, -1));
    return 0.5f * glm::vec3(normal.x + 1.0f, normal.y + 1.0f, normal.z + 1.0f); // Use normal to create a color
  }

  glm::dvec3 unit_direction = glm::normalize(ray.direction());
  float t = 0.5f * ((float)unit_direction.y + 1.0f); // Map y component to [0, 1]
  return (1.0f - t) * glm::vec3(1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f); // Gradient from white to blue for the background
}
