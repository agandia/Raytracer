
#include "Utilities.hpp"
#include "HitPool.hpp"
#include "Sphere.hpp"

int main() {
  // Image
  const double aspect_ratio = 16.0 / 9.0;
  const int image_width = 400;

  // Calculate the image height, but make sure that is at least 1.
  int image_height = int(image_width / aspect_ratio);
  image_height = (image_height < 1) ? 1 : image_height;

  // World
  HitPool world; // Create a hit pool to hold the hittables
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, 0.0, -1.0), 0.5)); // Add a sphere at (0, 0, -1) with radius 0.5
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, -100.5, -1.0), 100.0)); // Add a large sphere to act as the ground

  // Camera
  const double focal_length = 1.0; // Distance from camera to focal plane
  const double viewport_height = 2.0; // Height of the viewport
  const double viewport_width = viewport_height * (double(image_width)/image_height); // Width of the viewport
  const glm::dvec3 camera_center(0.0); // Camera position

  // Calculate two vectors across the horizontal and down the vertical viewport edges
  const glm::dvec3 viewport_u = glm::vec3(viewport_width, 0.0, 0.0); // Horizontal vector
  const glm::dvec3 viewport_v = glm::vec3(0.0, -viewport_height, 0.0); // Vertical vector

  // Calculate the  horizontal and vertical delta vetors from pixel to pixel
  const glm::dvec3 pixel_delta_u = viewport_u / double(image_width); // Horizontal delta vector
  const glm::dvec3 pixel_delta_v = viewport_v / double(image_height); // Vertical delta vector

  // Calculate the location of of the upper left pixel
  const glm::dvec3 viewport_upper_left = camera_center - glm::dvec3(0.0, 0.0, focal_length) - viewport_u / 2.0 - viewport_v / 2.0;
  const glm::dvec3 pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  //Render
  std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

  for (int j = 0; j < image_height; j++) {
    std::clog << "\rScanlines remaining: " << image_height - j << ' ' << std::flush;
    for (int i = 0; i < image_width; i++) {
      const glm::dvec3 pixel_center = pixel00_loc + (double)i * pixel_delta_u + (double)j * pixel_delta_v;
      const glm::dvec3 ray_direction = pixel_center - camera_center; // Ray direction from camera to pixel center
      Ray ray(camera_center, ray_direction);

      glm::vec3 color = ray_color(ray, world); // Get the color for the ray

      write_color(std::cout, color);
    }
  }
  std::clog << "\rDone.                   \n";
}