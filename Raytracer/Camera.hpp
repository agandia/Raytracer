#pragma once

#include "Hittable.hpp"

class Camera {
public:

  double  aspect_ratio      = 1.0;  // Ratio of image width over height
  int     image_width       = 100;  // Rendered image width in pixels
  int     samples_per_pixel = 100;  // Number of samples per pixel
  int     max_depth         = 10;   // Maximum number of ray bounces into the scene

  void render(const Hittable& world);

private:
  int         image_height;   // Rendered image height
  float       pixel_samples_scale; // Color scale factor for a sum of pixel samples  
  glm::dvec3  center;         // Camera center
  glm::dvec3  pixel00_loc;    // Location of pixel 0, 0
  glm::dvec3  pixel_delta_u;  // Offset to pixel to the right
  glm::dvec3  pixel_delta_v;  // Offset to pixel below

  void initialize();

  Ray get_ray(int x, int y) const;
  glm::dvec3 sample_square() const;
  glm::vec3 ray_color(const Ray& ray, int depth, const Hittable& world);

};