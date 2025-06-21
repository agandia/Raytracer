
#include "Camera.hpp"
#include "HitPool.hpp"
#include "Sphere.hpp"
#include "Material.hpp"
#include <memory>

int main() {
  std::shared_ptr<Lambertian> material_ground = std::make_shared<Lambertian>(glm::vec3(0.5, 0.5, 0.5)); // Create a gray Lambertian material for the scene ground
  std::shared_ptr<Lambertian> material_center = std::make_shared<Lambertian>(glm::vec3(0.1, 0.2, 0.5)); // Create a blue Lambertian material on the center sphere
  std::shared_ptr<Dielectric> material_left = std::make_shared<Dielectric>(1.5); // Create a dielectric material for the left sphere
  std::shared_ptr<Dielectric> material_bubble = std::make_shared<Dielectric>(1.0 / 1.5); // Create a dielectric material for the bubble sphere (ior represents glass outside and "air" inside)
  std::shared_ptr<Metal> material_back = std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2), 1.0); // Create a yellowish metallic material with high fuzziness on the back right sphere
  std::shared_ptr<Metal> material_right = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8), 0.1); // Create a blueish metallic material with low fuzziness
  
  // World
  HitPool world; // Create a hit pool to hold the hittables
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, -1000.5, -1.0), 1000.0, material_ground)); // Add a large sphere to act as the ground
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, 0.0, -1.2), 0.5, material_center));
  world.add(std::make_shared<Sphere>(glm::dvec3(-1.0, 0.0, -1.0), 0.5, material_left));
  world.add(std::make_shared<Sphere>(glm::dvec3(-1.0, 0.0, -1.0), 0.4, material_bubble));
  world.add(std::make_shared<Sphere>(glm::dvec3(2.0, 1.0, -3.0), 1.5, material_back));
  world.add(std::make_shared<Sphere>(glm::dvec3(1.0, 0.0, -1.0), 0.5, material_right));

  for (int a = -10; a < 10; a++) {
    for (int b = -10; b < 10; b++) {
      auto choose_mat = random_double();
      glm::dvec3 center(a + 0.9 * random_double(), 0.2, b + 0.9 * random_double());

      if ((center - glm::dvec3(4.0, 0.2, 0.0)).length() > 0.9) {
        std::shared_ptr<Material> sphere_material;

        if (choose_mat < 0.8) {
          // diffuse
          auto albedo = random() * random();
          sphere_material = std::make_shared<Lambertian>(albedo);
          world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
        }
        else if (choose_mat < 0.95) {
          // metal
          auto albedo = random(0.5, 1);
          auto fuzz = random_double(0, 0.5);
          sphere_material = std::make_shared<Metal>(albedo, fuzz);
          world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
        }
        else {
          // glass
          sphere_material = std::make_shared<Dielectric>(1.5);
          world.add(std::make_shared<Sphere>(center, 0.2, sphere_material));
        }
      }
    }
  }

  // Camera
  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0; // Set the aspect ratio to 16:9
  cam.image_width = 1920;
  cam.samples_per_pixel = 500; // Set the number of samples per pixel for anti-aliasing
  cam.max_depth = 50; // Set the maximum recursion depth for ray tracing
  
  cam.vertical_fov = 20.0;
  cam.look_from = glm::dvec3(18.0, 4.0, 5.0); // Set the camera position
  cam.look_at = glm::dvec3(0.0, 0.0, 0.0); // Set the point the camera is looking at
  cam.view_up = glm::dvec3(0.0, 1.0, 0.0); // Set the up direction for the camera

  cam.defocus_angle = 0.1;
  cam.focus_distance = 10.0;

  cam.render(world);
}