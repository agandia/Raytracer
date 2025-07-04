#include <iostream>
#include <fstream>
#include <chrono>
#include <omp.h>
#include <string>

#include "Camera.hpp"
#include "Utilities.hpp"
#include "Material.hpp"
#include "PDF.hpp"


void Camera::render(const Hittable& world, const Hittable& lights) {
  initialize();

  std::vector<glm::vec3> framebuffer(image_width * image_height);

  auto start = std::chrono::high_resolution_clock::now();

#pragma omp parallel for schedule(dynamic, 1)
  for (int j = 0; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      glm::vec3 pixel_color(0, 0, 0);
      for (int s_j = 0; s_j < sqrt_spp; ++s_j) {
        for (int s_i = 0; s_i < sqrt_spp; ++s_i) {
          Ray r = get_ray(i, j, s_i, s_j);
          pixel_color += ray_color(r, max_depth, world, lights);
        }
      }
      framebuffer[j * image_width + i] = (float)pixel_samples_scale * pixel_color;
    }

#pragma omp critical
    {
      std::clog << "\rScanlines remaining: " << (image_height - j) << ' ' << std::flush;
    }
  }

  std::cout << "P3\n" << image_width << ' ' << image_height << "\n255\n";
  for (int j = 0; j < image_height; ++j) {
    for (int i = 0; i < image_width; ++i) {
      write_color(std::cout, framebuffer[j * image_width + i]);
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

  sqrt_spp = int(std::sqrt(samples_per_pixel));
  pixel_samples_scale = 1.0 / (sqrt_spp * sqrt_spp);
  recip_sqrt_spp = 1.0 / sqrt_spp;

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

Ray Camera::get_ray(int i, int j, int s_i, int s_j) const {
  // Construct a camera ray originating from the defocus disk and directed at a randomly
  // sampled point around the pixel location i, j for stratified sample square s_i, s_j.

  glm::dvec3 offset = sample_square_stratified(s_i, s_j);
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

glm::dvec3 Camera::sample_square_stratified(int i, int j) const
{
  // Returns the vector to a random point in the square sub-pixel specified by grid
  // indices s_i and s_j, for an idealized unit square pixel [-.5,-.5] to [+.5,+.5].

  auto px = ((i + random_double()) * recip_sqrt_spp) - 0.5;
  auto py = ((j + random_double()) * recip_sqrt_spp) - 0.5;

  return glm::dvec3(px, py, 0);
}

glm::vec3 Camera::ray_color(const Ray& ray, int depth, const Hittable& world, const Hittable& lights) const {
  // If we've exceeded the ray bounce limit, no more light is gathered.
  if(depth <= 0) {
    return glm::vec3(0.0f); // Return black color
  }

  HitRecord hit_record;
  // Add a small delta to the interval to avoid self-intersection (shadow acne)
  if (!world.hit(ray, Interval(0.001, infinity), hit_record)) {
    // If the ray does not hit anything, return the background color
    //std::cout << "Ray missed world! Origin: " << std::to_string(ray.origin().x) << ", " << std::to_string(ray.origin().y) << "," << std::to_string(ray.origin().z)
    //  << ", dir: " << std::to_string(ray.direction().x) << "," << std::to_string(ray.direction().y) << "," << std::to_string(ray.direction().z) <<"\n";

    return background;
  }

  ScatterRecord scatter_record;
  glm::vec3 emitted_color = hit_record.material->emitted(ray, hit_record, hit_record.u, hit_record.v, hit_record.p);

  if(!hit_record.material->scatter(ray, hit_record, scatter_record)) {
    // If the material scatters the ray, recursively calculate the color
    return emitted_color;
  }

  if (scatter_record.skip_pdf) {
    return scatter_record.attenuation * ray_color(scatter_record.skip_pdf_ray, depth - 1, world, lights);
  }

  std::shared_ptr<HittablePDF> light_ptr = std::make_shared<HittablePDF>(lights, hit_record.p);
  MixturePDF p(light_ptr, scatter_record.pdf_ptr);

  Ray scattered_ray = Ray(hit_record.p, p.generate(), ray.time());
  double pdf_value = p.value(scattered_ray.direction());
  if (pdf_value < 1e-6) return emitted_color; // Avoids singularities

  double scattering_pdf = hit_record.material->scattering_pdf(ray, hit_record, scattered_ray);
  
  glm::vec3 sample_color = ray_color(scattered_ray, depth - 1, world, lights);
  glm::vec3 scattered_color = (scatter_record.attenuation * (float)scattering_pdf * sample_color) / (float)pdf_value;

  return emitted_color + scattered_color; // Combine emitted color and scattered color
}

glm::dvec3 Camera::defocus_disk_sample() const
{
  // Returns a random point in the camera defocus disk.
  glm::dvec3 p = random_in_unit_disk();
  return center + (p.x * defocus_disk_u) + (p.y * defocus_disk_v);
}
