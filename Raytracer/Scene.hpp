#pragma once

#include "BVH.hpp"
#include "Camera.hpp"
#include "ConstantMedium.hpp"
#include "HitPool.hpp"
#include "Shapes/Shapes.hpp"
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

      if (glm::length(center - glm::dvec3(4.0, 0.2, 0.0)) > 0.9) {
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

  // Quad Light
  std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(glm::vec3(4, 4, 4));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 10, 0), glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 1), light));
  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(0, 10, 0), glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 1), empty_material));

  world = HitPool(std::make_shared<BVHNode>(world)); // Create a BVH from the hit pool for efficient ray tracing

  // Camera
  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0; // Set the aspect ratio to 16:9
  cam.image_width = 400;
  cam.samples_per_pixel = 50; // Set the number of samples per pixel for anti-aliasing
  cam.max_depth = 50; // Set the maximum recursion depth for ray tracing
  cam.background = glm::vec3(0.7, 0.8, 1.0); // Set the background color to a light blue

  cam.vertical_fov = 20.0;
  cam.look_from = glm::dvec3(18.0, 4.0, 5.0); // Set the camera position
  cam.look_at = glm::dvec3(0.0, 0.0, 0.0); // Set the point the camera is looking at
  cam.view_up = glm::dvec3(0.0, 1.0, 0.0); // Set the up direction for the camera

  cam.defocus_angle = 0.1;
  cam.focus_distance = 10.0;

  cam.render(world, lights);
}

void checkered_spheres() {
  // World
  HitPool world; // Create pool to hold  all the objects in the scene that can be hit by rays

  std::shared_ptr<CheckerTexture> checkered_material = std::make_shared<CheckerTexture>(0.32, glm::vec3(0.2, 0.3, 0.1), glm::vec3(0.9, 0.9, 0.9)); // Create a checkered green and white material for the scene ground
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, -10.0, 0.0), 10.0, std::make_shared<Lambertian>(checkered_material)));
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0,  10.0, 0.0), 10.0, std::make_shared<Lambertian>(checkered_material)));
  // Quad Light
  std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(glm::vec3(4, 4, 4));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 5, 0), glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 1), light));
  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(0, 5, 0), glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 1), empty_material));

  // Camera
  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0; // Set the aspect ratio to 16:9
  cam.image_width = 400;
  cam.samples_per_pixel = 100; // Set the number of samples per pixel for anti-aliasing
  cam.max_depth = 50; // Set the maximum recursion depth for ray tracing
  cam.background = glm::vec3(0.7, 0.8, 1.0); // Set the background color to a light blue

  cam.vertical_fov = 20.0;
  cam.look_from = glm::dvec3(13.0, 2.0, 3.0); // Set the camera position
  cam.look_at = glm::dvec3(0.0, 0.0, 0.0); // Set the point the camera is looking at
  cam.view_up = glm::dvec3(0.0, 1.0, 0.0); // Set the up direction for the camera

  cam.defocus_angle = 0.0;
  
  cam.render(world, lights);
}

void earth() {
  // World
  HitPool world; // Create pool to hold  all the objects in the scene that can be hit by rays

  std::shared_ptr<ImageTexture> earth_texture= std::make_shared<ImageTexture>("earthmap.jpg"); // Create a image from file
  std::shared_ptr<Lambertian> earth_surface = std::make_shared<Lambertian>(earth_texture);
  world.add(std::make_shared<Sphere>(glm::dvec3(0.0, 0.0, 0.0), 2.0, earth_surface));

  // Quad Light
  std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(glm::vec3(4, 4, 4));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 5, 0), glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 1), light));
  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(0, 5, 0), glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 1), empty_material));

  // Camera
  Camera cam;
  cam.aspect_ratio = 16.0 / 9.0; // Set the aspect ratio to 16:9
  cam.image_width = 400;
  cam.samples_per_pixel = 100; // Set the number of samples per pixel for anti-aliasing
  cam.max_depth = 50; // Set the maximum recursion depth for ray tracing
  cam.background = glm::vec3(0.7, 0.8, 1.0); // Set the background color to a light blue

  cam.vertical_fov = 20.0;
  cam.look_from = glm::dvec3(0.0, 0.0, 12.0); // Set the camera position
  cam.look_at = glm::dvec3(0.0, 0.0, 0.0); // Set the point the camera is looking at
  cam.view_up = glm::dvec3(0.0, 1.0, 0.0); // Set the up direction for the camera

  cam.defocus_angle = 0.0;

  cam.render(world, lights);
}

void perlin_spheres() {
  HitPool world;

  auto perlintext = std::make_shared<NoiseTexture>(4.0);
  world.add(std::make_shared<Sphere>(glm::dvec3(0, -1000, 0), 1000, std::make_shared<Lambertian>(perlintext)));
  world.add(std::make_shared<Sphere>(glm::dvec3(0, 2, 0), 2, std::make_shared<Lambertian>(perlintext)));

  // Quad Light
  std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(glm::vec3(4, 4, 4));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 5, 0), glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 1), light));
  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(0, 5, 0), glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 1), empty_material));

  Camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.background = glm::vec3(0.7, 0.8, 1.0); // Set the background color to a light blue

  cam.vertical_fov = 20;
  cam.look_from = glm::dvec3(13, 2, 3);
  cam.look_at = glm::dvec3(0, 0, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, lights);
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
  //glm::dvec3 b0(-2.0, 0.0, 0.0);
  //glm::dvec3 b1(2.0, 0.0, 0.0);
  //glm::dvec3 b2(0.0, 3.0, 0.0);
  //glm::dvec3 base_mid = 0.5 * (b0 + b1);
  //world.add(std::make_shared<Triangle>(b0, b1-b0, b2 - b0 - base_mid, left_red));
  //world.add(pyramid(glm::dvec3(0, 0, 0), glm::dvec3(4, 0, 0), glm::dvec3(0, 0, -4), 4, back_green));
   
  //world.add(std::make_shared<Ellipse>(glm::dvec3(2, 5, 3), glm::dvec3(4, 0, 0), glm::dvec3(0, 4, 0), back_green));
  // They get expensive very vert quickly
  world.add(std::make_shared<Cylindroid>(glm::dvec3(0, 0, 0), glm::dvec3(4, 0, 0), glm::dvec3(0, 0, -4), 8, 32, back_green));

  world.add(std::make_shared<Quad>(glm::dvec3(-3, -2, 5), glm::dvec3(0, 0, -4), glm::dvec3(0, 4, 0), left_red));
  world.add(std::make_shared<Quad>(glm::dvec3(-2, -2, 0), glm::dvec3(4, 0, 0), glm::dvec3(0, 4, 0), back_green));
  world.add(std::make_shared<Quad>(glm::dvec3(3, -2, 1), glm::dvec3(0, 0, 4), glm::dvec3(0, 4, 0), right_blue));
  world.add(std::make_shared<Quad>(glm::dvec3(-2, 3, 1), glm::dvec3(4, 0, 0), glm::dvec3(0, 0, 4), upper_orange));
  world.add(std::make_shared<Quad>(glm::dvec3(-2, -3, 5), glm::dvec3(4, 0, 0), glm::dvec3(0, 0, -4), lower_teal));

  // Quad Light
  std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(glm::vec3(4, 4, 4));
  world.add(std::make_shared<Quad>(glm::dvec3(-1, 0, -1), glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 1), light));
  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(-1, 0, -1), glm::dvec3(1, 0, 0), glm::dvec3(0, 0, 1), empty_material));

  Camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.background = glm::vec3(0.7, 0.8, 1.0); // Set the background color to a light blue

  cam.vertical_fov = 80;
  cam.look_from = glm::dvec3(0, 10, 9);
  cam.look_at = glm::dvec3(0, 0, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, lights);
}

void simple_light() {
  HitPool world;

  // Materials
  auto perlintext = std::make_shared<NoiseTexture>(4.0);
  world.add(std::make_shared<Sphere>(glm::dvec3(0, -1000, 0), 1000, std::make_shared<Lambertian>(perlintext)));
  world.add(std::make_shared<Sphere>(glm::dvec3(0, 2, 0), 2, std::make_shared<Lambertian>(perlintext)));

  // Quad Light
  std::shared_ptr<DiffuseLight> light = std::make_shared<DiffuseLight>(glm::vec3(4, 4, 4));  
  world.add(std::make_shared<Quad>(glm::dvec3(3, 1, -2), glm::dvec3(2, 0, 0), glm::dvec3(0, 2, 0), light));

  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(3, 1, -2), glm::dvec3(2, 0, 0), glm::dvec3(0, 2, 0), empty_material));

  Camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 400;
  cam.samples_per_pixel = 100;
  cam.max_depth = 50;
  cam.background = glm::vec3(0.f, 0.f, 0.f); // Set the background color to a light blue

  cam.vertical_fov = 20;
  cam.look_from = glm::dvec3(26, 3, 6);
  cam.look_at = glm::dvec3(0, 2, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, lights);
}

void cornell_box() {
  HitPool world;

  // Materials
  auto red = std::make_shared<Lambertian>(glm::vec3(.65, 0.05, 0.05));
  auto white = std::make_shared<Lambertian>(glm::vec3(0.73, 0.73, 0.73));
  auto green = std::make_shared<Lambertian>(glm::vec3(0.12, 0.45, 0.15));
  auto light = std::make_shared<DiffuseLight>(glm::vec3(15.f, 15.f, 15.f));

  // Quads
  //world.add(std::make_shared<Triangle>(glm::dvec3(343, 0, 332), glm::dvec3(0, 0, 130), glm::dvec3(0, 130, 0), red));
  world.add(std::make_shared<Quad>(glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), green));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), red));
  world.add(std::make_shared<Quad>(glm::dvec3(0,0,0), glm::dvec3(555, 0, 0), glm::dvec3(0, 0, 555), white));
  world.add(std::make_shared<Quad>(glm::dvec3(555, 555, 555), glm::dvec3(-555, 0, 0), glm::dvec3(0, 0, -555), white));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 555), glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), white));

  //Light
  world.add(std::make_shared<Quad>(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0), glm::dvec3(0, 0, -105), light));

  std::shared_ptr<Material> aluminium = std::make_shared<Metal>(glm::vec3(0.8, 0.85, 0.88), 0.01);
  std::shared_ptr<Hittable> box1 = std::make_shared<Box>(glm::dvec3(0, 0, 0), glm::dvec3(165, 330, 165), aluminium);
  box1 = make_shared<RotateYAxis>(box1, 15);
  box1 = make_shared<Translate>(box1, glm::vec3(265, 0, 295));
  world.add(box1);

  //std::shared_ptr<Hittable> box2 = box(glm::dvec3(0, 0, 0), glm::dvec3(165, 165, 165), white);
  //box2 = make_shared<RotateYAxis>(box2, -18);
  //box2 = make_shared<Translate>(box2, glm::vec3(130, 0, 65));
  //world.add(box2);

  // Glass Sphere
  std::shared_ptr<Dielectric> glass = std::make_shared<Dielectric>(1.5);
  world.add(std::make_shared<Sphere>(glm::dvec3(190, 90, 190), 90, glass));

  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0), glm::dvec3(0, 0, -105), empty_material));
  lights.add(std::make_shared<Sphere>(glm::dvec3(190,90,190), 90, empty_material));

  Camera cam;

  cam.aspect_ratio = 1;
  cam.image_width = 600;
  cam.samples_per_pixel = 1000;
  cam.max_depth = 50;
  cam.background = glm::vec3(0.0, 0.0, 0.0); // Black background

  cam.vertical_fov = 40;
  cam.look_from = glm::dvec3(278, 278, -800);
  cam.look_at = glm::dvec3(278, 278, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, lights);
}

void cornell_smoke() {
  HitPool world;

  // Materials
  auto red = std::make_shared<Lambertian>(glm::vec3(.65, 0.05, 0.05));
  auto white = std::make_shared<Lambertian>(glm::vec3(0.73, 0.73, 0.73));
  auto green = std::make_shared<Lambertian>(glm::vec3(0.12, 0.45, 0.15));
  auto light = std::make_shared<DiffuseLight>(glm::vec3(15.f, 15.f, 15.f));

  // Quads
  world.add(std::make_shared<Quad>(glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), green));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), red));
  world.add(std::make_shared<Quad>(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0), glm::dvec3(0, 0, -105), light));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 0), glm::dvec3(555, 0, 0), glm::dvec3(0, 0, 555), white));
  world.add(std::make_shared<Quad>(glm::dvec3(555, 555, 555), glm::dvec3(-555, 0, 0), glm::dvec3(0, 0, -555), white));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 555), glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), white));

  std::shared_ptr<Hittable> box1 = std::make_shared<Box>(glm::dvec3(0, 0, 0), glm::dvec3(165, 330, 165), white);
  box1 = make_shared<RotateYAxis>(box1, 15);
  box1 = make_shared<Translate>(box1, glm::vec3(265, 0, 295));
  world.add(box1);

  std::shared_ptr<Hittable> box2 = std::make_shared<Box>(glm::dvec3(0, 0, 0), glm::dvec3(165, 165, 165), white);
  box2 = make_shared<RotateYAxis>(box2, -18);
  box2 = make_shared<Translate>(box2, glm::vec3(130, 0, 65));
  world.add(box2);

  world.add(std::make_shared<ConstantMedium>(box1, 0.01, glm::vec3(0.0)));
  world.add(std::make_shared<ConstantMedium>(box2, 0.01, glm::vec3(1.0)));

  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0), glm::dvec3(0, 0, -105), empty_material));

  Camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 600;
  cam.samples_per_pixel = 200;
  cam.max_depth = 50;
  cam.background = glm::vec3(0.0, 0.0, 0.0); // Black background

  cam.vertical_fov = 40;
  cam.look_from = glm::dvec3(278, 278, -800);
  cam.look_at = glm::dvec3(278, 278, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, lights);
}

void final_scene(int image_width, int samples_per_pixel, int max_depth) {
  HitPool boxes1;
  auto ground = std::make_shared<Lambertian>(glm::vec3(0.48, 0.83, 0.53));

  int boxes_per_side = 20;
  for (int i = 0; i < boxes_per_side; i++) {
    for (int j = 0; j < boxes_per_side; j++) {
      auto w = 100.0;
      auto x0 = -1000.0 + i * w;
      auto z0 = -1000.0 + j * w;
      auto y0 = 0.0;
      auto x1 = x0 + w;
      auto y1 = random_double(1, 101);
      auto z1 = z0 + w;

      boxes1.add(std::make_shared<Box>(glm::dvec3(x0, y0, z0), glm::dvec3(x1, y1, z1), ground));
    }
  }

  HitPool world;

  world.add(std::make_shared<BVHNode>(boxes1));

  auto light = std::make_shared<DiffuseLight>(glm::vec3(7, 7, 7));
  world.add(std::make_shared<Quad>(glm::dvec3(123, 554, 147), glm::dvec3(300, 0, 0), glm::dvec3(0, 0, 265), light));

  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(123, 554, 147), glm::dvec3(300, 0, 0), glm::dvec3(0, 0, 265), empty_material));

  auto center1 = glm::dvec3(400, 400, 200);
  auto center2 = center1 + glm::dvec3(30, 0, 0);
  auto sphere_material = std::make_shared<Lambertian>(glm::vec3(0.7, 0.3, 0.1));
  world.add(std::make_shared<Sphere>(center1, center2, 50, sphere_material));

  world.add(std::make_shared<Sphere>(glm::dvec3(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
  world.add(std::make_shared<Sphere>(
    glm::dvec3(0, 150, 145), 50, std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.9), 1.0)
  ));

  auto boundary = std::make_shared<Sphere>(glm::dvec3(360, 150, 145), 70, std::make_shared<Dielectric>(1.5));
  world.add(boundary);
  world.add(std::make_shared<ConstantMedium>(boundary, 0.2, glm::vec3(0.2, 0.4, 0.9)));
  boundary = std::make_shared<Sphere>(glm::dvec3(0, 0, 0), 5000, std::make_shared<Dielectric>(1.5));
  world.add(std::make_shared<ConstantMedium>(boundary, .0001, glm::vec3(1, 1, 1)));

  auto emat = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("earthmap.jpg"));
  world.add(std::make_shared<Sphere>(glm::dvec3(400, 200, 400), 100, emat));
  auto pertext = std::make_shared<NoiseTexture>(0.2);
  world.add(std::make_shared<Sphere>(glm::dvec3(220, 280, 300), 80, std::make_shared<Lambertian>(pertext)));

  HitPool boxes2;
  auto white = std::make_shared<Lambertian>(glm::vec3(.73, .73, .73));
  int ns = 1000;
  for (int j = 0; j < ns; j++) {
    boxes2.add(std::make_shared<Sphere>(random(0, 165), 10, white));
  }

  world.add(std::make_shared<Translate>(
    std::make_shared<RotateYAxis>(
      std::make_shared<BVHNode>(boxes2), 15),
    glm::dvec3(-100, 270, 395)
  )
  );

  Camera cam;

  cam.aspect_ratio = 1.0;
  cam.image_width = image_width;
  cam.samples_per_pixel = samples_per_pixel;
  cam.max_depth = max_depth;
  cam.background = glm::vec3(0, 0, 0);

  cam.vertical_fov = 40;
  cam.look_from = glm::dvec3(478, 278, -600);
  cam.look_at = glm::dvec3(278, 278, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, lights);
}

void boosted_scene(int image_width, int samples_per_pixel, int max_depth) {
  HitPool world;
  HitPool lights;

  // Ground using grid of boxes
  HitPool boxes1;
  std::shared_ptr<Lambertian> green_grass = std::make_shared<Lambertian>(glm::vec3(0.48, 0.83, 0.53));
  int boxes_per_side = 20;
  for (int i = 0; i < boxes_per_side; i++) {
    for (int j = 0; j < boxes_per_side; j++) {
      double w = 100;
      double x0 = -1000 + i * w;
      double z0 = -1000 + j * w;
      double y1 = random_double(1, 101);
      boxes1.add(std::make_shared<Box>(glm::dvec3(x0, 0, z0), glm::dvec3(x0 + w, y1, z0 + w), green_grass));
    }
  }
  world.add(std::make_shared<BVHNode>(boxes1));

  // Textures and materials
  auto noise_texture = std::make_shared<NoiseTexture>(0.2);
  auto noise = std::make_shared<Lambertian>(noise_texture); // x
  auto earth_texture = std::make_shared<ImageTexture>("earthmap.jpg");
  auto earth = std::make_shared<Lambertian>(earth_texture); // x
  auto glass = std::make_shared<Dielectric>(1.5); // x
  auto brushed_metal = std::make_shared<Metal>(glm::vec3(0.8, 0.85, 0.88), 0.01); // x
  auto bronze = std::make_shared<Lambertian>(glm::vec3(0.6, 0.3, 0.1)); // x
  auto red = std::make_shared<Lambertian>(glm::vec3(0.65, 0.05, 0.05));
  auto white = std::make_shared<Lambertian>(glm::vec3(0.73, 0.73, 0.73)); // x
  auto green = std::make_shared<Lambertian>(glm::vec3(0.12, 0.45, 0.15));
  auto blue_frost = std::make_shared<Isotropic>(glm::vec3(0.6, 0.6, 1.0)); // x
  auto porcelain = std::make_shared<Lambertian>(glm::vec3(0.95, 0.92, 0.9)); // x
  auto asphalt = std::make_shared<Lambertian>(glm::vec3(0.15, 0.15, 0.17)); // x
  auto charcoal_metal = std::make_shared<Metal>(glm::vec3(0.2, 0.2, 0.2), 0.0); // x
  auto bronze_metal = std::make_shared<Metal>(glm::vec3(0.8, 0.6, 0.2), 0.05); // x
  auto cyan = std::make_shared<Lambertian>(glm::vec3(0.2, 0.8, 0.9)); // x
  auto magenta = std::make_shared<Lambertian>(glm::vec3(0.9, 0.2, 0.7)); // x

  // Subsurface materials
  auto wax = std::make_shared<SubsurfaceMaterial>(glm::dvec3(1.2, 1.0, 0.8), glm::dvec3(0.002, 0.004, 0.01), 0.0, 1.44);
  auto milk = std::make_shared<SubsurfaceMaterial>(glm::dvec3(2.5), glm::dvec3(0.01), 0.0, 1.3);
  auto jade = std::make_shared<SubsurfaceMaterial>(glm::dvec3(0.7, 0.9, 0.5), glm::dvec3(0.001, 0.002, 0.0015), 0.0, 1.5);
  auto marble = std::make_shared<SubsurfaceMaterial>(glm::dvec3(2.19, 2.62, 3.0), glm::dvec3(0.0021, 0.0041, 0.0071), 0.0, 1.5); 
  auto skin = std::make_shared<SubsurfaceMaterial>(glm::dvec3(0.74, 0.88, 1.01), glm::dvec3(0.032, 0.17, 0.48), 0.9, 1.40);

  auto empty_mat = std::shared_ptr<Material>();

  // Overhead light
  auto light = std::make_shared<DiffuseLight>(glm::vec3(7, 7, 7));
  auto pin_light = std::make_shared<DiffuseLight>(glm::vec3(15, 15, 15));
  auto ceiling_light = std::make_shared<Quad>(glm::dvec3(123, 554, 147), glm::dvec3(300, 0, 0), glm::dvec3(0, 0, 265), light);
  world.add(ceiling_light);
  lights.add(std::make_shared<Quad>(glm::dvec3(123, 554, 147), glm::dvec3(300, 0, 0), glm::dvec3(0, 0, 265), empty_mat));
  // Back light to enhance sss qualities
  auto back_light = std::make_shared<Sphere>(glm::dvec3(450, 220, 0), 25, pin_light);
  world.add(back_light);
  lights.add(std::make_shared<Sphere>(glm::dvec3(500, 150, 0), 25, empty_mat));

  // Add Scene shapes
  world.add(std::make_shared<Sphere>(glm::dvec3(400, 200, 400), 100, earth)); // Textured Earth sphere
  world.add(std::make_shared<Sphere>(glm::dvec3(400, 400, 200), glm::dvec3(430, 400, 200), 50, bronze)); // Moving bronce ball with 2 centers (c1, c2, r, mat)
  world.add(std::make_shared<Sphere>(glm::dvec3(260, 150, 45), 50, glass)); // Glass Sphere
  world.add(std::make_shared<Sphere>(glm::dvec3(0, 150, 145), 50, bronze_metal)); // Bronce Metallic Sphere
  world.add(std::make_shared<Sphere>(glm::dvec3(220, 280, 300), 80, porcelain)); // Center sphere with a porcelain like lambertian
  world.add(std::make_shared<Sphere>(glm::dvec3(-150, 330, 350), 40, noise)); // Sphere using Noise texture

  // Glass sphere with blue smoke inside
  auto foggy_boundary = std::make_shared<Sphere>(glm::dvec3(360, 150, 145), 70, glass);
  world.add(foggy_boundary);
  world.add(std::make_shared<ConstantMedium>(foggy_boundary, 0.2, glm::vec3(0.2, 0.4, 0.9))); // ConstantMedium(center, density, isotropic_color)

  // Particle cloud
  HitPool boxes2;
  for (int j = 0; j < 1000; ++j) {
    boxes2.add(std::make_shared<Sphere>(random(0, 165), 10, white));
  }
  world.add(std::make_shared<Translate>(std::make_shared<RotateYAxis>(std::make_shared<BVHNode>(boxes2), 15), glm::dvec3(-100, 270, 395)));

  // Pyramid using isotropic blue frost
  auto pyramid = std::make_shared<Pyramid>(glm::dvec3(-270, 180, 375), glm::dvec3(60, 0, 0), glm::dvec3(0, 0, -60), 80, blue_frost);
  world.add(std::make_shared<RotateYAxis>(pyramid, 15));
  
  world.add(std::make_shared<Pyramid>(glm::dvec3(270, 354, 300), glm::dvec3(60, 0, 0), glm::dvec3(0, 0, -60), 100.0, glass)); // Glass Pyramid (Origin, u, v, height, mat)

  auto sss_cone = std::make_shared<Cone>(glm::dvec3(450, 150, -190), glm::dvec3(30, 0, 0), glm::dvec3(0, 0, -30), 80, 32, milk);
  world.add(sss_cone);

  world.add(std::make_shared<Cone>(glm::dvec3(0, 550, 250), glm::dvec3(-30, 0, 0), glm::dvec3(0, 0, -30), 80, 32, red));

  // Cylindroid: asphalt and charcoal
  world.add(std::make_shared<Cylindroid>(glm::dvec3(100, 50, 100), glm::dvec3(20, 0, 0), glm::dvec3(0, 0, -20), 80, 16, asphalt));
  world.add(std::make_shared<Cylindroid>(glm::dvec3(150, 50, 100), glm::dvec3(20, 0, 0), glm::dvec3(0, 0, -20), 80, 16, charcoal_metal));

  // Interesting 2D shapes with colorful Lambertian materials
  world.add(std::make_shared<Ellipse>(glm::dvec3(140, 70, 350), glm::dvec3(15, 0, 0), glm::dvec3(0, 25, 0),  cyan)); // Ellipse
  world.add(std::make_shared<Triangle>(glm::dvec3(210, 70, 360), glm::dvec3(30, 0, 10), glm::dvec3(15, 40, 0), magenta)); // Triangle corners as (A, B-A, C-A) or (A, u, v)

  // Fog box above
  auto fog_box = std::make_shared<Box>(glm::dvec3(-1000, 500, -1000), glm::dvec3(1000, 550, 1000), std::make_shared<Isotropic>(glm::vec3(0.7, 0.7, 0.8)));
  world.add(std::make_shared<ConstantMedium>(fog_box, 0.01, glm::vec3(1.0)));

  //std::shared_ptr<Hittable> box = std::make_shared<Box>(glm::dvec3(400.0, 100.0, -230.0), glm::dvec3(450.0, 200.0, -180.0), jade);  // mid‑left box
  //box = make_shared<RotateYAxis>(box, 1);
  //world.add(box);

  // Camera
  Camera cam;
  cam.aspect_ratio = 1.0;
  cam.image_width = image_width;
  cam.samples_per_pixel = samples_per_pixel;
  cam.max_depth = max_depth;
  cam.background = glm::vec3(0, 0, 0);
  cam.vertical_fov = 45;
  cam.look_from = glm::dvec3(478, 278, -720);
  cam.look_at = glm::dvec3(278, 278, 0);
  cam.view_up = glm::dvec3(0, 1, 0);
  cam.defocus_angle = 0;

  cam.render(world, lights);
}

void glass_pyr_test() {

  HitPool world;

  // Materials
  auto red = std::make_shared<Lambertian>(glm::vec3(.65, 0.05, 0.05));
  auto white = std::make_shared<Lambertian>(glm::vec3(0.73, 0.73, 0.73));
  auto green = std::make_shared<Lambertian>(glm::vec3(0.12, 0.45, 0.15));
  auto light = std::make_shared<DiffuseLight>(glm::vec3(40.f));

  // Quads
  world.add(std::make_shared<Quad>(glm::dvec3(-250, 0, 0), glm::dvec3(0, 500, 0), glm::dvec3(0, 0, -500), green)); // left
  world.add(std::make_shared<Quad>(glm::dvec3(250, 0, 0), glm::dvec3(0, 500, 0), glm::dvec3(0, 0, -500), red)); // right
  
  world.add(std::make_shared<Quad>(glm::dvec3(-250, 0, 0), glm::dvec3(500, 0, 0), glm::dvec3(0, 0, -500), white)); // floor
  world.add(std::make_shared<Quad>(glm::dvec3(-250, 500, 0), glm::dvec3(500, 0, 0), glm::dvec3(0, 0, -500), white)); // ceiling
  auto dark = std::make_shared<Lambertian>(glm::vec3(0.1, 0.1, 0.1));
  world.add(std::make_shared<Quad>(glm::dvec3(-250, 0, -500), glm::dvec3(500, 0, 0), glm::dvec3(0, 500, 0), dark)); // back

  world.add(std::make_shared<Quad>(
    glm::dvec3(-50, 499, -300),
    glm::dvec3(100, 0, 0),
    glm::dvec3(0, 0, 100),
    light)); // light object

  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>( glm::dvec3(-50, 499, -300), glm::dvec3(100, 0, 0), glm::dvec3(0, 0, 100), empty_material));

  world.add(std::make_shared<Sphere>(glm::dvec3(50, 100, -250), 50, white));
  
  // very sneaky sneaky cheeky way to make a glass pyramid by adding a (hemi)sphere at the top, avoiding numerical issues with the pyramid planes
  auto glass = std::make_shared<Dielectric>(1.5);

  // Pyramid base parameters
  glm::dvec3 base_origin = glm::dvec3(-50, 10, -100);
  glm::dvec3 u = glm::dvec3(100, 0, 0);
  glm::dvec3 v = glm::dvec3(0, 0, -100);
  double height = 200.0;

  // Add the glass pyramid body
  world.add(std::make_shared<Pyramid>(base_origin, u, v, height, glass));

  // Compute apex position
  glm::dvec3 apex = base_origin + 0.5 * u + 0.5 * v + glm::dvec3(0, height, 0);

  // Add a small hemisphere (modeled as a full sphere) at the tip
  double tip_radius = 2.5;
  world.add(std::make_shared<Sphere>(apex - glm::dvec3(0, tip_radius, 0), tip_radius, glass));


  Camera cam;

  cam.aspect_ratio = 16.0 / 9.0;
  cam.image_width = 600;
  cam.samples_per_pixel = 1000;
  cam.max_depth = 50;
  cam.background = glm::vec3(0.0, 0.0, 0.0); // Black background

  cam.vertical_fov = 40;
  cam.look_from = glm::dvec3(0, 278, 600);
  cam.look_at = glm::dvec3(0, 278, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, lights);
}

void prob_dens_func_test() {

  HitPool world;

  // Materials
  auto red = std::make_shared<Lambertian>(glm::vec3(.65, 0.05, 0.05));
  auto white = std::make_shared<Lambertian>(glm::vec3(0.73, 0.73, 0.73));
  auto green = std::make_shared<Lambertian>(glm::vec3(0.12, 0.45, 0.15));
  auto light = std::make_shared<DiffuseLight>(glm::vec3(15.f, 15.f, 15.f));

  // Quads
  //world.add(std::make_shared<Triangle>(glm::dvec3(343, 0, 332), glm::dvec3(0, 0, 130), glm::dvec3(0, 130, 0), red));
  world.add(std::make_shared<Quad>(glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), green));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), red));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 0), glm::dvec3(555, 0, 0), glm::dvec3(0, 0, 555), white));
  world.add(std::make_shared<Quad>(glm::dvec3(555, 555, 555), glm::dvec3(-555, 0, 0), glm::dvec3(0, 0, -555), white));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 555), glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), white));

  //Light
  world.add(std::make_shared<Quad>(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0), glm::dvec3(0, 0, -105), light));
    // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0), glm::dvec3(0, 0, -105), empty_material));
  
  std::shared_ptr<Material> aluminium = std::make_shared<Metal>(glm::vec3(0.8, 0.85, 0.88), 0.01); 
  
  //std::shared_ptr<Hittable> box2 = box(glm::dvec3(0, 0, 0), glm::dvec3(165, 165, 165), white);
  //box2 = make_shared<RotateYAxis>(box2, -18);
  //box2 = make_shared<Translate>(box2, glm::vec3(130, 0, 65));
  //world.add(box2);

  //world.add(std::make_shared<Triangle>(glm::dvec3(390, 0, 295), glm::dvec3(-260, 0 , 0), glm::dvec3(-130, 295, 0), light));
  std::shared_ptr<Hittable> test_box = std::make_shared<Box>(
    glm::dvec3(190, 0, 190),              // base min corner
    glm::dvec3(280, 165, 280),            // top max corner (90 units high box)
    aluminium                             // non-light material
  );
  test_box = std::make_shared<RotateYAxis>(test_box, 15);
  
  world.add(test_box);
  
  std::shared_ptr<Hittable> test_box_light = std::make_shared<Box>(
    glm::dvec3(190, 0, 190),              // base min corner
    glm::dvec3(280, 165, 280),            // top max corner (90 units high box)
    empty_material// non-light material
  );
  test_box_light = std::make_shared<RotateYAxis>(test_box_light, 15);
  lights.add(test_box_light);
  
  auto test_cone = std::make_shared<Cone>(
    glm::dvec3(378, 0, 278),           // Base centered in Cornell Box
    glm::dvec3(65, 0, 0),              // u = 65 units wide
    glm::dvec3(0, 0, -65),              // v = -65 units deep so the cross product points up
    120,                               // Height up to 120
    32,
    aluminium
  );
  world.add(test_cone);
  
  
  auto test_cone_light = std::make_shared<Cone>(
    glm::dvec3(378, 0, 278),           // Base centered in Cornell Box
    glm::dvec3(65, 0, 0),              // u = 65 units wide
    glm::dvec3(0, 0, -65),              // v = 65 units deep
    120,                               // Height up to 120
    32,
    empty_material
  );
  lights.add(test_cone_light);

  std::shared_ptr<Hittable> test_pyramid = std::make_shared<Pyramid>(
    glm::dvec3(178, 0, 278),           // Base centered in Cornell Box
    glm::dvec3(65, 0, 0),              // u = 65 units wide
    glm::dvec3(0, 0, -65),              // v = -65 units deep so the cross product points up
    120,                               // Height up to 120
    aluminium
  );
  //test_pyramid = std::make_shared<RotateYAxis>(test_pyramid, 15);
  world.add(test_pyramid);

  std::shared_ptr<Hittable> test_pyramid_light = std::make_shared<Pyramid>(
    glm::dvec3(178, 0, 278),           // Base centered in Cornell Box
    glm::dvec3(65, 0, 0),              // u = 65 units wide
    glm::dvec3(0, 0, -65),              // v = 65 units deep
    120,                               // Height up to 120
    empty_material
  );
  lights.add(test_pyramid_light);

  Camera cam;

  cam.aspect_ratio = 1;
  cam.image_width = 600;
  cam.samples_per_pixel = 1000;
  cam.max_depth = 50;
  cam.background = glm::vec3(0.0, 0.0, 0.0); // Black background

  cam.vertical_fov = 40;
  cam.look_from = glm::dvec3(278, 278, -800);
  cam.look_at = glm::dvec3(278, 278, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, lights);
}

void dipole_diffusion_profile_test() {

  HitPool world;

  // Materials
  auto red = std::make_shared<Lambertian>(glm::vec3(.65, 0.05, 0.05));
  auto white = std::make_shared<Lambertian>(glm::vec3(0.73, 0.73, 0.73));
  auto green = std::make_shared<Lambertian>(glm::vec3(0.12, 0.45, 0.15));
  auto gray = std::make_shared<Lambertian>(glm::vec3(0.3, 0.3, 0.3));
  auto bright_light = std::make_shared<DiffuseLight>(glm::vec3(80.f, 80.f, 80.f));
  auto dim_light = std::make_shared<DiffuseLight>(glm::vec3(15.f, 15.f, 15.f));

  // Quads forming the cornell box
  world.add(std::make_shared<Quad>(glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), green));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), red));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 0), glm::dvec3(555, 0, 0), glm::dvec3(0, 0, 555), gray));
  world.add(std::make_shared<Quad>(glm::dvec3(555, 555, 555), glm::dvec3(-555, 0, 0), glm::dvec3(0, 0, -555), gray));
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 555), glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), gray));

  // Top Light
  world.add(std::make_shared<Quad>(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0), glm::dvec3(0, 0, -105), dim_light));
  // Light source behind SSS sphere (bright small sphere)
  world.add(std::make_shared<Sphere>(glm::dvec3(375, 100, 430), 30.0, bright_light));
  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0), glm::dvec3(0, 0, -105), empty_material));
  lights.add(std::make_shared<Sphere>(glm::dvec3(375, 100, 430), 30.0, empty_material));

  // Reference: plain red Lambertian
  auto lambertian_orange = std::make_shared<Lambertian>(glm::vec3(1.0, 0.5, 0.3));

  // -- Alternative Wax preset 
  glm::dvec3 sigma_s_mm(1.2, 1.0, 0.8); // scattering
  glm::dvec3 sigma_a_mm(0.002, 0.004, 0.01); // absorption

  auto wax_mat = std::make_shared<SubsurfaceMaterial>(sigma_s_mm, sigma_a_mm, 0.0, 1.44);             // g, eta
  // Place two spheres side-by-side
  world.add(std::make_shared<Sphere>(glm::dvec3(175, 100, 270), 50.0, lambertian_orange));
  //world.add(std::make_shared<Sphere>(glm::dvec3(375, 100, 270), 50.0, wax_mat));

  std::shared_ptr<Hittable> box1 = std::make_shared<Box>(glm::dvec3(300, 75, 245), glm::dvec3(400, 275, 345), wax_mat);
  box1 = make_shared<RotateYAxis>(box1, 15);
  world.add(box1);

  Camera cam;

  cam.aspect_ratio = 1;
  cam.image_width = 600;
  cam.samples_per_pixel = 25000;
  cam.max_depth = 512;
  cam.background = glm::vec3(0.0, 0.0, 0.0); // Black background

  cam.vertical_fov = 40;
  cam.look_from = glm::dvec3(278, 278, -800);
  cam.look_at = glm::dvec3(278, 278, 0);
  cam.view_up = glm::dvec3(0, 1, 0);

  cam.defocus_angle = 0;

  cam.render(world, lights);
}

void sss_gallery()  // drop into main.cpp
{
  HitPool world, lights;

  /* -- Cornell walls --------------------------------------------------- */
  auto red = std::make_shared<Lambertian>(glm::vec3(.65, .05, .05));
  auto green = std::make_shared<Lambertian>(glm::vec3(.12, .45, .15));
  auto gray = std::make_shared<Lambertian>(glm::vec3(.30, .30, .30));

  world.add(std::make_shared<Quad>(glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), green)); // left
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 0), glm::dvec3(0, 555, 0), glm::dvec3(0, 0, 555), red));   // right
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 0), glm::dvec3(555, 0, 0), glm::dvec3(0, 0, 555), gray));  // floor
  world.add(std::make_shared<Quad>(glm::dvec3(555, 555, 555), glm::dvec3(-555, 0, 0), glm::dvec3(0, 0, -555), gray));  // ceiling
  world.add(std::make_shared<Quad>(glm::dvec3(0, 0, 555), glm::dvec3(555, 0, 0), glm::dvec3(0, 555, 0), gray));  // back

  /* -- Lights ---------------------------------------------------------- */
  auto light_panel = std::make_shared<DiffuseLight>(glm::vec3(25, 25, 25));
  auto back_light = std::make_shared<DiffuseLight>(glm::vec3(15, 15, 15));   // bright
  auto front_light = std::make_shared<DiffuseLight>(glm::vec3(5, 5, 5));   // dim

  // ceiling panel (normal -y)
  world.add(std::make_shared<Quad>(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0),
    glm::dvec3(0, 0, -105), light_panel));
  lights.add(std::make_shared<Quad>(glm::dvec3(343, 554, 332), glm::dvec3(-130, 0, 0),
    glm::dvec3(0, 0, -105), nullptr));

  // back sphere light
  world.add(std::make_shared<Sphere>(glm::dvec3(278, 278, 520), 20, back_light));
  lights.add(std::make_shared<Sphere>(glm::dvec3(278, 278, 520), 20, nullptr));

  /* -- Subsurface presets --------------------------------------------- */
  auto wax = std::make_shared<SubsurfaceMaterial>(
    glm::dvec3(0.7, 0.7, 0.7), glm::dvec3(0.002, 0.003, 0.006), 0.8, 1.44);

  auto jade = std::make_shared<SubsurfaceMaterial>(
    glm::dvec3(2.19, 2.62, 3.0), glm::dvec3(0.0021, 0.0041, 0.0071), 0.8, 1.45);

  auto marble = std::make_shared<SubsurfaceMaterial>(
    glm::dvec3(2.9, 2.37, 1.45), glm::dvec3(0.0029, 0.003, 0.004), 0.9, 1.48);

  auto milk = std::make_shared<SubsurfaceMaterial>(
    glm::dvec3(0.7, 1.2, 1.4), glm::dvec3(0.0014, 0.0025, 0.0142), 0.0, 1.34);

  auto skin = std::make_shared<SubsurfaceMaterial>(
    glm::dvec3(0.74, 0.88, 1.01), glm::dvec3(0.032, 0.17, 0.48), 0.9, 1.40);

  /* -- Gallery objects ------------------------------------------------- */
  // Coordinates are centre positions in mm
  world.add(std::make_shared<Sphere>(glm::dvec3(90, 115, 275), 60, skin));         // front‑left

  std::shared_ptr<Hittable> box = std::make_shared<Box>(glm::dvec3(200.0, 10.0, 150.0), glm::dvec3(300.0, 210.0, 250.0), jade);  // mid‑left box
  box = make_shared<RotateYAxis>(box, 15);
  world.add(box);

  std::shared_ptr<Hittable> pyramid = std::make_shared<Pyramid>(glm::dvec3(375.0, 10.0, 275.0),   // front-right
    glm::dvec3(80, 0, 0), glm::dvec3(0, 0, -80),
    120, wax);
  pyramid = make_shared<RotateYAxis>(pyramid, 22);
  world.add(pyramid);

  std::shared_ptr<Hittable> cone = std::make_shared<Cone  >(glm::dvec3(190.0, 10.0, 380.0),    // back-left
    glm::dvec3(50, 0, 0), glm::dvec3(0, 0, -50),
    140, 32, marble);                           // cone
  // Warning: this is hacky AF, cone faces looking at the camera are exactly backlit by the back light, 
  // making the shape black bc exiting rays capture the black background, adding a small or 0 rotation, fixes that (!??) go figure...
  cone = make_shared<RotateYAxis>(cone, 0); 
  world.add(cone);

  std::shared_ptr<Hittable> cylinder = std::make_shared<Cylindroid>(glm::dvec3(425, 10, 380),// back‑right
    glm::dvec3(60, 0, 0), glm::dvec3(0, 0, -60),
    160, 48, milk);
  // Warning: hackity hack.
  cylinder = make_shared<RotateYAxis>(cylinder, 0);
  world.add(cylinder);

  /* -- Camera ---------------------------------------------------------- */
  Camera cam;
  cam.aspect_ratio = 1.0;
  cam.image_width = 600;
  cam.samples_per_pixel = 160;   // good default; bump if still noisy
  cam.max_depth = 400;
  cam.background = glm::vec3(0);
  cam.vertical_fov = 40;
  cam.look_from = glm::dvec3(278, 278, -800);
  cam.look_at = glm::dvec3(278, 278, 0);
  cam.view_up = glm::dvec3(0, 1, 0);
  cam.defocus_angle = 0;

  cam.render(world, lights);
}
