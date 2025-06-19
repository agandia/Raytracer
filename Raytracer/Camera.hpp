#pragma once

#include "Hittable.hpp"

class Camera {
public:

  double aspect_ratio = 1.0; // Ratio of image width over height
  int image_width = 100; // Rendered image width in pixels

  void render(const Hittable& world);

private:
  int       image_height;   // Rendered image height
  glm::dvec3 center;         // Camera center
  glm::dvec3 pixel00_loc;    // Location of pixel 0, 0
  glm::dvec3 pixel_delta_u;  // Offset to pixel to the right
  glm::dvec3 pixel_delta_v;  // Offset to pixel below

  void initialize();

  glm::vec3 ray_color(const Ray& ray, const Hittable& world);

};