
#include "Camera.hpp"
#include "HitPool.hpp"
#include "Sphere.hpp"

int main() {
  // World
  HitPool world; // Create a hit pool to hold the hittables
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, 0.0, -1.0), 0.5)); // Add a sphere at (0, 0, -1) with radius 0.5
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, -100.5, -1.0), 100.0)); // Add a large sphere to act as the ground

  // Camera
  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0; // Set the aspect ratio to 16:9
  cam.image_width = 400;
  cam.max_depth = 50; // Set the maximum recursion depth for ray tracing

  cam.render(world);
}