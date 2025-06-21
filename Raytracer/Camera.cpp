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

  center = look_from; // Set the camera position at the origin

  // Determine the viewport dimensions.
  //const double theta = degrees_to_radians(vertical_fov);
  const double theta = glm::radians(vertical_fov);
  const double h = std::tan(theta / 2);
  const double viewport_height = 2.0 * h * focus_distance; // Height of the viewport
  const double viewport_width = viewport_height * (double(image_width) / image_height); // Width of the viewport

  // Calculate the u,v,w basis vectors for the camera coordinate system
  w = glm::normalize(look_from - look_at); // Direction from camera to focal point
  u = glm::normalize(glm::cross(view_up, w)); // Perpendicular vector to w in the up direction
  v = glm::cross(w, u); // Perpendicular vector to both w and u

  // Calculate two vectors across the horizontal and down the vertical viewport edges
  const glm::dvec3 viewport_u = viewport_width * u; // Horizontal vector
  const glm::dvec3 viewport_v = viewport_height * -v; // Vertical vector

  // Calculate the  horizontal and vertical delta vectors from pixel to pixel
  pixel_delta_u = viewport_u / double(image_width); // Horizontal delta vector
  pixel_delta_v = viewport_v / double(image_height); // Vertical delta vector

  // Calculate the location of of the upper left pixel
  const glm::dvec3 viewport_upper_left = center - (focus_distance * w) - viewport_u / 2.0 - viewport_v / 2.0;
  pixel00_loc = viewport_upper_left + 0.5 * (pixel_delta_u + pixel_delta_v);

  // Calculate the camera defocus disk basis vectors
  double defocus_radius = focus_distance * glm::tan(degrees_to_radians(defocus_angle / 2.0));
  defocus_disk_u = defocus_radius * u; // Horizontal defocus vector
  defocus_disk_v = defocus_radius * v; // Vertical defocus vector
}

Ray Camera::get_ray(int i, int j) const {
  // Construct a camera ray originating from the defocus disk and directed at a randomly
  // sampled point around the pixel location i, j.

  glm::dvec3 offset = sample_square();
  glm::dvec3 pixel_sample = pixel00_loc
    + ((i + offset.x) * pixel_delta_u)
    + ((j + offset.y) * pixel_delta_v);

  auto ray_origin = (defocus_angle <= 0.0) ? center : defocus_disk_sample();
  auto ray_direction = pixel_sample - ray_origin;

  return Ray(ray_origin, ray_direction);
}

glm::dvec3 Camera::sample_square() const {
  //Returns the vector to a random point in the [-0.5,-0.5]-[+0.5,+0.5] square.
  return glm::dvec3(random_double() - 0.5, random_double() - 0.5, 0.0);
}

glm::vec3 Camera::ray_color(const Ray& ray, int depth, const Hittable& world) const {
  // If we've exceeded the ray bounce limit, no more light is gathered.
  if(depth <= 0) {
    return glm::vec3(0.0f); // Return black color
  }

  HitRecord hit_record;
  // Add a small delta to the interval to avoid self-intersection (shadow acne)
  if (world.hit(ray, Interval(0.00001, infinity), hit_record)) {
    Ray scattered_ray;
    glm::vec3 attenuation;
    if(hit_record.material->scatter(ray, hit_record, attenuation, scattered_ray)) {
      // If the material scatters the ray, recursively calculate the color
      return attenuation * ray_color(scattered_ray, depth - 1, world);
    }
    return glm::vec3(0.0f); // If the material does not scatter, return black
  }

  glm::dvec3 unit_direction = glm::normalize(ray.direction());
  float t = 0.5f * ((float)unit_direction.y + 1.0f); // Map y component to [0, 1]
  return (1.0f - t) * glm::vec3(1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f); // Gradient from white to blue for the background
}

glm::dvec3 Camera::defocus_disk_sample() const
{
  // Returns a random point in the camera defocus disk.
  glm::dvec3 p = random_in_unit_disk();
  return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
}
