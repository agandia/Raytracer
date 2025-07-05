//#include "Scene.hpp"

#include <omp.h>
#include "simple_sss_cornell.hpp"

// Main ------------------------------------------------------------------------
int run_program() {
  const double aspect = 1.0; const int img_w = 800; const int img_h = static_cast<int>(img_w / aspect);
  const int samples = 25000; const int max_depth = 250;

  camera cam(glm::dvec3(1.5, 1, -3),   // look‑from (z < 0, outside box)
    glm::dvec3(1.5, 1, 1),   // look‑at centre
    glm::dvec3(0, 1, 0),      // up
    40,               // vfov
    1.0);             // aspect

  auto world = cornell_box();
  glm::vec3 bg{ 0,0,0 };


  std::vector<glm::vec3> framebuffer(img_w * img_h);

  auto start = std::chrono::high_resolution_clock::now();

  std::cout << "P3\n" << img_w << ' ' << img_h << "\n255\n";

#pragma omp parallel for schedule(dynamic,1)
  for (int j = 0; j < img_h; ++j) {
    for (int i = 0; i < img_w; ++i) {
      glm::vec3 col(0, 0, 0);
      for (int s = 0; s < samples; ++s) {
        double u = (i + rnd()) / (img_w - 1);
        double v = (j + rnd()) / (img_h - 1);
        col += trace(cam.get_ray(u, v), bg, world, max_depth);
      }
      float scale = 1.f / samples;
      col.x = std::sqrt(scale * col.x);
      col.y = std::sqrt(scale * col.y);
      col.z = std::sqrt(scale * col.z);

      framebuffer[(img_h - 1 - j) * img_w + i] = col;
    }
#pragma omp critical
    {
      static int scanned = 0;
      ++scanned;
      std::clog << "\rScanline " << img_h - scanned << " " << std::flush;
    }
  }

  for (const glm::vec3& c : framebuffer) {
    std::cout << static_cast<int>(256 * clamp(c.x, 0, 0.999)) << ' '
      << static_cast<int>(256 * clamp(c.y, 0, 0.999)) << ' '
      << static_cast<int>(256 * clamp(c.z, 0, 0.999)) << '\n';
  }

  auto end = std::chrono::high_resolution_clock::now();
  std::chrono::duration<double> elapsed = end - start;
  std::clog << "\nDone in " << elapsed.count() << " seconds.\n";
  return 1;
}


int main() {
  run_program();
//switch (13) {
  //case 1: bouncing_spheres();  break;
  //case 2: checkered_spheres(); break;
  //case 3: earth(); break;
  //case 4: perlin_spheres(); break;
  //case 5: quads(); break;
  //case 6: simple_light(); break;
  //case 7: cornell_box(); break;
  //case 8: cornell_smoke(); break;
  //case 9: final_scene(400, 250, 4); break;
  //case 10: final_scene(1920, 10000, 40); break;
  //case 11: glass_pyr_test(); break;
  //case 12: prob_dens_func_test(); break;
  //case 13: dipole_diffusion_profile_test(); break;
  //default: boosted_scene(800, 5000, 50); break;
  //default: boosted_scene(800, 100, 50); break;
  //}
}