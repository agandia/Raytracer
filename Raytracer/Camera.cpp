#include "Camera.hpp"

void Camera::render(const Hittable& world) {
  initialize();
  //Render
  std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

  for (int j = 0; j < image_height; j++) {
    std::clog << "\rScanlines remaining: " << image_height - j << ' ' << std::flush;
    for (int i = 0; i < image_width; i++) {
      const glm::dvec3 pixel_center = pixel00_loc + (double)i * pixel_delta_u + (double)j * pixel_delta_v;
      const glm::dvec3 ray_direction = pixel_center - center; // Ray direction from camera to pixel center
      Ray ray(center, ray_direction);

      glm::vec3 color = ray_color(ray, world); // Get the color for the ray

      write_color(std::cout, color);
    }
  }
  std::clog << "\rDone.                   \n";
}

void Camera::initialize() {
  // Calculate the image height, but make sure that is at least 1.
  image_height = int(image_width / aspect_ratio);
  image_height = (image_height < 1) ? 1 : image_height;

  center = glm::dvec3(0.0); // Set the camera position at the origin

  // Determine the viewport dimensions.
  const double focal_length = 1.0; // Distance from camera to focal plane
  const double viewport_height = 2.0; // Height of the viewport
  const double viewport_width = viewport_height * (double(image_width) / image_height); // Width of the viewport

// Calculate two vectors across the horizontal and down the vertical viewport edges
  const glm::dvec3 viewport_u = glm::vec3(viewport_width, 0.0, 0.0); // Horizontal vector
  const glm::dvec3 viewport_v = glm::vec3(0.0, -viewport_height, 0.0); // Vertical vector

  // Calculate the  horizontal and vertical delta vectors from pixel to pixel
  pixel_delta_u = viewport_u / double(image_width); // Horizontal delta vector
  pixel_delta_v = viewport_v / double(image_height); // Vertical delta vector

  // Calculate the location of of the upper left pixel
  const glm::dvec3 viewport_upper_left = center - glm::dvec3(0.0, 0.0, focal_length) - viewport_u / 2.0 - viewport_v / 2.0;
  pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);
}

glm::vec3 Camera::ray_color(const Ray& ray, const Hittable& world) {
  HitRecord hit_record;
  if (world.hit(ray, Interval(0, infinity), hit_record)) {
    return 0.5f * (glm::vec3(hit_record.normal) + glm::vec3(1.0f));
  }

  glm::dvec3 unit_direction = glm::normalize(ray.direction());
  float t = 0.5f * ((float)unit_direction.y + 1.0f); // Map y component to [0, 1]
  return (1.0f - t) * glm::vec3(1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f); // Gradient from white to blue for the background
}