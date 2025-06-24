#pragma once

#include "BVH.hpp"
#include "Camera.hpp"
#include "HitPool.hpp"
#include "Sphere.hpp"
#include "Quad.hpp"
#include "Material.hpp"
#include "TextureWrapper.hpp"
#include <memory>

void bouncing_spheres() {
  std::shared_ptr<CheckerTexture> material_ground = std::make_shared<CheckerTexture>(0.32, glm::vec3(0.2, 0.3, 0.1), glm::vec3(0.9, 0.9, 0.9)); // Create a checkered green and white material for the scene ground
  std::shared_ptr<Lambertian> material_center = std::make_shared<Lambertian>(glm::vec3(0.1, 0.2, 0.5)); // Create a blue Lambertian material on the center sphere
  std::shared_ptr<Dielectric> material_left = std::make_shared<Dielectric>(1.5); // Create a dielectric material for the left sphere
  std::shared_ptr<Dielectric> material_bubble = std::make_shared<Dielectric>(1.0 / 1.5); // Create a dielectric material for the bubble sphere (ior represents glass outside and "air" inside)
  std::shared_ptr<Metal> material_back = std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2), 1.0); // Create a yellowish metallic material with high fuzziness on the back right sphere
  std::shared_ptr<Metal> material_right = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.8), 0.1); // Create a blueish metallic material with low fuzziness

  // World
  HitPool world; // Create a hit pool to hold the hittables
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, -1000.5, -1.0), 1000.0, std::make_shared<Lambertian>(material_ground))); // Add a large sphere to act as the ground
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
          glm::dvec3 center_t2 = center + glm::dvec3(0, random_double(0, 0.5), 0);
          world.add(std::make_shared<Sphere>(center, center_t2, 0.2, sphere_material));
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

  world = HitPool(std::make_shared<BVHNode>(world)); // Create a BVH from the hit pool for efficient ray tracing

  // Camera
  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0; // Set the aspect ratio to 16:9
  cam.image_width = 400;
  cam.samples_per_pixel = 50; // Set the number of samples per pixel for anti-aliasing
  cam.max_depth = 50; // Set the maximum recursion depth for ray tracing

  cam.vertical_fov = 20.0;
  cam.look_from = glm::dvec3(18.0, 4.0, 5.0); // Set the camera position
  cam.look_at = glm::dvec3(0.0, 0.0, 0.0); // Set the point the camera is looking at
  cam.view_up = glm::dvec3(0.0, 1.0, 0.0); // Set the up direction for the camera

  cam.defocus_angle = 0.1;
  cam.focus_distance = 10.0;

  cam.render(world);
}

void checkered_spheres() {
  // World
  HitPool world; // Create pool to hold  all the objects in the scene that can be hit by rays

  std::shared_ptr<CheckerTexture> checkered_material = std::make_shared<CheckerTexture>(0.32, glm::vec3(0.2, 0.3, 0.1), glm::vec3(0.9, 0.9, 0.9)); // Create a checkered green and white material for the scene ground
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, -10.0, 0.0), 10.0, std::make_shared<Lambertian>(checkered_material)));
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0,  10.0, 0.0), 10.0, std::make_shared<Lambertian>(checkered_material)));
  
  // Camera
  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0; // Set the aspect ratio to 16:9
  cam.image_width = 400;
  cam.samples_per_pixel = 100; // Set the number of samples per pixel for anti-aliasing
  cam.max_depth = 50; // Set the maximum recursion depth for ray tracing

  cam.vertical_fov = 20.0;
  cam.look_from = glm::dvec3(13.0, 2.0, 3.0); // Set the camera position
  cam.look_at = glm::dvec3(0.0, 0.0, 0.0); // Set the point the camera is looking at
  cam.view_up = glm::dvec3(0.0, 1.0, 0.0); // Set the up direction for the camera

  cam.defocus_angle = 0.0;
  
  cam.render(world);
}

void earth() {
  // World
  HitPool world; // Create pool to hold  all the objects in the scene that can be hit by rays

  std::shared_ptr<ImageTexture> earth_texture= std::make_shared<ImageTexture>("earthmap.jpg"); // Create a image from file
  std::shared_ptr<Lambertian> earth_surface = std::make_shared<Lambertian>(earth_texture);
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, 0.0, 0.0), 2.0, earth_surface));

  // Camera
  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0; // Set the aspect ratio to 16:9
  cam.image_width = 400;
  cam.samples_per_pixel = 100; // Set the number of samples per pixel for anti-aliasing
  cam.max_depth = 50; // Set the maximum recursion depth for ray tracing

  cam.vertical_fov = 20.0;
  cam.look_from = glm::dvec3(0.0, 0.0, 12.0); // Set the camera position
  cam.look_at = glm::dvec3(0.0, 0.0, 0.0); // Set the point the camera is looking at
  cam.view_up = glm::dvec3(0.0, 1.0, 0.0); // Set the up direction for the camera

  cam.defocus_angle = 0.0;

  cam.render(world);
}

void perlin_spheres() {
  HitPool world;

  auto perlintext = std::make_shared<NoiseTexture>(4.0);
  world.add(std::make_shared<Sphere>(glm::dvec3(0, -1000, 0), 1000, std::make_shared<Lambertian>(perlintext)));
  world.add(std::make_shared<Sphere>(glm::dvec3(0, 2, 0), 2, std::make_shared<Lambertian>(perlintext)));

  Camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;

  cam.vertical_fov = 20;
  cam.look_from = glm::dvec3(13, 2, 3);
  cam.look_at = glm::dvec3(0, 0, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world);
}

void quads() {
  HitPool world;

  // Materials
  auto left_red = std::make_shared<Lambertian>(glm::vec3(1.0, 0.2, 0.2));
  auto back_green = std::make_shared<Lambertian>(glm::vec3(0.2, 1.0, 0.2));
  auto right_blue = std::make_shared<Lambertian>(glm::vec3(0.2, 0.2, 1.0));
  auto upper_orange = std::make_shared<Lambertian>(glm::vec3(1.0, 0.5, 0.0));
  auto lower_teal = std::make_shared<Lambertian>(glm::vec3(0.2, 0.8, 0.8));

  // Quads
  world.add(std::make_shared<Quad>(glm::dvec3(-3, -2, 5), glm::dvec3(0, 0, -4), glm::dvec3(0, 4, 0), left_red));
  world.add(std::make_shared<Quad>(glm::dvec3(-2, -2, 0), glm::dvec3(4, 0, 0), glm::dvec3(0, 4, 0), back_green));
  world.add(std::make_shared<Quad>(glm::dvec3(3, -2, 1), glm::dvec3(0, 0, 4), glm::dvec3(0, 4, 0), right_blue));
  world.add(std::make_shared<Quad>(glm::dvec3(-2, 3, 1), glm::dvec3(4, 0, 0), glm::dvec3(0, 0, 4), upper_orange));
  world.add(std::make_shared<Quad>(glm::dvec3(-2, -3, 5), glm::dvec3(4, 0, 0), glm::dvec3(0, 0, -4), lower_teal));

  Camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;

  cam.vertical_fov = 80;
  cam.look_from = glm::dvec3(0,0, 9);
  cam.look_at = glm::dvec3(0, 0, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world);
}