#include "Camera.hpp"
#include "Utilities.hpp"
#include <iostream>

void Camera::render(const Hittable& world) {
  initialize();
  //Render
  std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

  for (int j = 0; j < image_height; j++) {
    std::clog << "\rScanlines remaining: " << image_height - j << ' ' << std::flush;
    for (int i = 0; i < image_width; i++) {
      glm::vec3 pixel_color(0.0f); // Initialize pixel color to black
      for(int sample = 0; sample < samples_per_pixel; sample++) {
        // Generate a ray for the current pixel
        Ray ray = get_ray(i, j);
        pixel_color += ray_color(ray, max_depth, world); // Accumulate color for the pixel
      }

      write_color(std::cout, pixel_samples_scale * pixel_color);
    }
  }
  std::clog << "\rDone.                   \n";
}

void Camera::initialize() {
  // Calculate the image height, but make sure that is at least 1.
  image_height = int(image_width / aspect_ratio);
  image_height = (image_height < 1) ? 1 : image_height;

  pixel_samples_scale = 1.0f / samples_per_pixel;

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

Ray Camera::get_ray(int i, int j) const {
  // Construct a camera ray originating from the origin and directed at randomly sampled
  // point around the pixel location i, j.

  glm::dvec3 offset = sample_square();
  glm::dvec3 pixel_sample = pixel00_loc
    + ((i + offset.x) * pixel_delta_u)
    + ((j + offset.y) * pixel_delta_v);

  auto ray_origin = center;
  auto ray_direction = pixel_sample - ray_origin;

  return Ray(ray_origin, ray_direction);
}

glm::dvec3 Camera::sample_square() const {
  //Returns the vector to a random point in the [-0.5,-0.5]-[+0.5,+0.5] square.
  return glm::dvec3(random_double() - 0.5, random_double() - 0.5, 0.0);
}

glm::vec3 Camera::ray_color(const Ray& ray, int depth, const Hittable& world) {
  // If we've exceeded the ray bounce limit, no more light is gathered.
  if(depth <= 0) {
    return glm::vec3(0.0f); // Return black color
  }

  HitRecord hit_record;
  // Add a small delta to the interval to avoid self-intersection (shadow acne)
  if (world.hit(ray, Interval(0.00001, infinity), hit_record)) {
    glm::dvec3 direction = hit_record + random_unit_vector();
    return 0.5f * ray_color(Ray(hit_record.p, direction), depth-1, world);
  }

  glm::dvec3 unit_direction = glm::normalize(ray.direction());
  float t = 0.5f * ((float)unit_direction.y + 1.0f); // Map y component to [0, 1]
  return (1.0f - t) * glm::vec3(1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f); // Gradient from white to blue for the background
}