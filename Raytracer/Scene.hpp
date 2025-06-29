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
  //world.add(cylindroid(glm::dvec3(0, 0, 0), glm::dvec3(4, 0, 0), glm::dvec3(0, 0, -4), 8, 32, back_green));

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
  HitPool boxes1;
  std::shared_ptr<Lambertian> ground = std::make_shared<Lambertian>(glm::vec3(0.48, 0.83, 0.53));

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
  // Offset cubes that make up the ground
  world.add(std::make_shared<BVHNode>(boxes1));

  auto light = std::make_shared<DiffuseLight>(glm::vec3(7, 7, 7));
  world.add(std::make_shared<Quad>(glm::dvec3(123, 554, 147), glm::dvec3(300, 0, 0), glm::dvec3(0, 0, 265), light));

  // Light Sources
  std::shared_ptr<Material> empty_material = std::shared_ptr<Material>();
  HitPool lights;
  lights.add(std::make_shared<Quad>(glm::dvec3(123, 554, 147), glm::dvec3(300, 0, 0), glm::dvec3(0, 0, 265), empty_material));

  // Bronce sphere with a Dynamic (moving) center, gives the impression of blurred motion.
  glm::dvec3 center1 = glm::dvec3(400, 400, 200);
  glm::dvec3 center2 = center1 + glm::dvec3(30, 0, 0);
  std::shared_ptr<Lambertian> bronce_material = std::make_shared<Lambertian>(glm::vec3(0.7, 0.3, 0.1)); // Create a Lambertian material with an orange / bronce color
  world.add(std::make_shared<Sphere>(center1, center2, 50, bronce_material));
  
  // Glass sphere center and front
  world.add(std::make_shared<Sphere>(glm::dvec3(260, 150, 45), 50, std::make_shared<Dielectric>(1.5)));
   
  // Metallic sphere on the right of the scene with a bit of fuzziness
  std::shared_ptr<Metal> metal_material = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.43), 1.0);
  world.add(std::make_shared<Sphere>(glm::dvec3(0, 150, 145), 50, metal_material));
  
  // This is a glass sphere just in fron of the earth textured sphere that contains a very saturated blue medium inside (blue smoke)
  std::shared_ptr<Dielectric> glass_material = std::make_shared<Dielectric>(1.5);
  std::shared_ptr<Sphere> boundary = std::make_shared<Sphere>(glm::dvec3(360, 150, 145), 70, glass_material);
  world.add(boundary);
  
  world.add(std::make_shared<ConstantMedium>(boundary, 0.2, glm::vec3(0.2, 0.4, 0.9)));
  
  // All the scene is enclosed in some sort of foggy or smoky medium (represented as a sphere with a large radius)
  //boundary = std::make_shared<Sphere>(glm::dvec3(0, 0, 0), 5000, std::make_shared<Dielectric>(1.5));
  //world.add(std::make_shared<ConstantMedium>(boundary, .0001, glm::vec3(1, 1, 1))); //This should be the white foggy medium
  
  // Textured sphere
  std::shared_ptr<Lambertian> earth_material = std::make_shared<Lambertian>(std::make_shared<ImageTexture>("earthmap.jpg"));
  world.add(std::make_shared<Sphere>(glm::dvec3(400, 200, 400), 100, earth_material));
  
  // Perlin noise sphere in the center of the scene
  std::shared_ptr<NoiseTexture> pertext = std::make_shared<NoiseTexture>(0.2);
  world.add(std::make_shared<Sphere>(glm::dvec3(220, 280, 300), 80, std::make_shared<Lambertian>(pertext)));

  // Polystyrene looking cloud, made ou of many small spheres
  HitPool boxes2;
  std::shared_ptr<Lambertian> white = std::make_shared<Lambertian>(glm::vec3(.73, .73, .73));
  int ns = 1000;
  for (int j = 0; j < ns; j++) {
    boxes2.add(std::make_shared<Sphere>(random(0, 165), 10, white));
  }
  // Should be the polystyrene volume because the cube is rotated
  world.add(std::make_shared<Translate>(std::make_shared<RotateYAxis>(std::make_shared<BVHNode>(boxes2), 15), glm::dvec3(-100, 270, 395)));

  // Rotated pyramid with bronce material
  HitPool pyram;
  pyram.add(std::make_shared<Pyramid>(glm::dvec3(0, 0, 0), glm::dvec3(60, 0, 0), glm::dvec3(0, 0, -60), 80, metal_material));
  world.add(std::make_shared<Translate>(std::make_shared<RotateYAxis>(std::make_shared<BVHNode>(pyram), 15), glm::dvec3(-150, 220, 445)));

  // Pyramid base parameters
  glm::dvec3 base_origin = glm::dvec3(270, 354, 300);
  glm::dvec3 u = glm::dvec3(60, 0, 0);
  glm::dvec3 v = glm::dvec3(0, 0, -60);
  double height = 100.0;

  // Add the glass pyramid body
  world.add(std::make_shared<Pyramid>(base_origin, u, v, height, glass_material));

  // Compute apex position
  //glm::dvec3 apex = base_origin + 0.5 * u + 0.5 * v + glm::dvec3(0, height, 0);

  // Add a small hemisphere (modeled as a full sphere) at the tip
  //double tip_radius = 2.5;
  //world.add(std::make_shared<Sphere>(apex - glm::dvec3(0, tip_radius, 0), tip_radius, glass_material));

  
  // White cylinder
  std::shared_ptr<Metal> polished_metal = std::make_shared<Metal>(glm::vec3(0.8, 0.8, 0.9), 0.01);
  world.add(std::make_shared<Cylindroid>(glm::dvec3(120, 50, 80), glm::dvec3(40, 0, 0), glm::dvec3(0, 0, -40), 100, 32, polished_metal));


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
  //std::shared_ptr<Hittable> test_box = std::make_shared<Box>(
  //  glm::dvec3(190, 0, 190),              // base min corner
  //  glm::dvec3(280, 165, 280),            // top max corner (90 units high box)
  //  aluminium                             // non-light material
  //);
  //test_box = std::make_shared<RotateYAxis>(test_box, 15);
  //
  //world.add(test_box);
  //
  //std::shared_ptr<Hittable> test_box_light = std::make_shared<Box>(
  //  glm::dvec3(190, 0, 190),              // base min corner
  //  glm::dvec3(280, 165, 280),            // top max corner (90 units high box)
  //  empty_material// non-light material
  //);
  //test_box_light = std::make_shared<RotateYAxis>(test_box_light, 15);
  //lights.add(test_box_light);
  
  //auto test_cone = std::make_shared<Cone>(
  //  glm::dvec3(278, 0, 278),           // Base centered in Cornell Box
  //  glm::dvec3(65, 0, 0),              // u = 65 units wide
  //  glm::dvec3(0, 0, -65),              // v = -65 units deep so the cross product points up
  //  120,                               // Height up to 120
  //  32,
  //  aluminium
  //);
  //world.add(test_cone);
  //
  //
  //auto test_cone_light = std::make_shared<Cone>(
  //  glm::dvec3(278, 0, 278),           // Base centered in Cornell Box
  //  glm::dvec3(65, 0, 0),              // u = 65 units wide
  //  glm::dvec3(0, 0, -65),              // v = 65 units deep
  //  120,                               // Height up to 120
  //  32,
  //  empty_material
  //);
  //lights.add(test_cone_light);

  std::shared_ptr<Hittable> test_pyramid = std::make_shared<Cylindroid>(
    glm::dvec3(278, 0, 278),           // Base centered in Cornell Box
    glm::dvec3(65, 0, 0),              // u = 65 units wide
    glm::dvec3(0, 0, -65),              // v = -65 units deep so the cross product points up
    120,                               // Height up to 120
    32,
    aluminium
  );
  //test_pyramid = std::make_shared<RotateYAxis>(test_pyramid, 15);
  world.add(test_pyramid);

  std::shared_ptr<Hittable> test_pyramid_light = std::make_shared<Cylindroid>(
    glm::dvec3(278, 0, 278),           // Base centered in Cornell Box
    glm::dvec3(65, 0, 0),              // u = 65 units wide
    glm::dvec3(0, 0, -65),              // v = 65 units deep
    120,                               // Height up to 120
    32,
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