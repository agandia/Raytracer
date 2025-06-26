#include <iostream>
#include <fstream>
#include <chrono>
#include <omp.h>


#include "Camera.hpp"
#include "Utilities.hpp"
#include "Material.hpp"


void Camera::render(const Hittable& world) {
  initialize();

  const int tile_size = 32;
  const int num_passes = 10;
  const int spp_per_pass = samples_per_pixel / num_passes;

  std::vector<glm::vec3> framebuffer(image_width * image_height, glm::vec3(0.0f));

  auto start = std::chrono::high_resolution_clock::now();

  for (int pass = 0; pass < num_passes; ++pass) {
    std::clog << "\nStarting pass " << (pass + 1) << "/" << num_passes << std::endl;

#pragma omp parallel for collapse(2) schedule(dynamic)
    for (int tile_y = 0; tile_y < image_height; tile_y += tile_size) {
      for (int tile_x = 0; tile_x < image_width; tile_x += tile_size) {
        for (int j = tile_y; j < std::min(tile_y + tile_size, image_height); ++j) {
          for (int i = tile_x; i < std::min(tile_x + tile_size, image_width); ++i) {
            glm::vec3 pixel_color(0.0f);
            for (int s = 0; s < spp_per_pass; ++s) {
              Ray ray = get_ray(i, j);
              pixel_color += ray_color(ray, max_depth, world);
            }
            framebuffer[j * image_width + i] += pixel_color;
          }
        }
      }
    }

    std::clog << "\rFinished pass " << (pass + 1) << "/" << num_passes << std::flush;
  }

  // Final write to image.ppm
  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";

  for (int j = 0; j < image_height ; ++j) {
    for (int i = 0; i < image_width; ++i) {
      glm::vec3 color = framebuffer[j * image_width + i] / float(samples_per_pixel);
      write_color(std::cout, color);
    }
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::clog << "\nDone in " << elapsed.count() << " seconds.\n";
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

  glm::dvec3 ray_origin = (defocus_angle <= 0.0) ? center : defocus_disk_sample();
  glm::dvec3 ray_direction = pixel_sample - ray_origin;
  double ray_time = random_double();


  return Ray(ray_origin, ray_direction, ray_time);
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
  if (!world.hit(ray, Interval(0.00001, infinity), hit_record)) {
    // If the ray does not hit anything, return the background color
    return background;
  }

  Ray scattered_ray;
  glm::vec3 attenuation;
  glm::vec3 emitted_color = hit_record.material->emitted(hit_record.u, hit_record.v, hit_record.p);

  if(!hit_record.material->scatter(ray, hit_record, attenuation, scattered_ray)) {
    // If the material scatters the ray, recursively calculate the color
    return emitted_color;
  }

  glm::vec3 scattered_color = attenuation * ray_color(scattered_ray, depth - 1, world);
  return emitted_color + scattered_color; // Combine emitted color and scattered color
  
  // Old code to have a blueish gradient background
  //glm::dvec3 unit_direction = glm::normalize(ray.direction());
  //float t = 0.5f * ((float)unit_direction.y + 1.0f); // Map y component to [0, 1]
  //return (1.0f - t) * glm::vec3(1.0f) + t * glm::vec3(0.5f, 0.7f, 1.0f); // Gradient from white to blue for the background
}

glm::dvec3 Camera::defocus_disk_sample() const
{
  // Returns a random point in the camera defocus disk.
  glm::dvec3 p = random_in_unit_disk();
  return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
}
