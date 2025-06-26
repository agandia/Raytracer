#include "Scene.hpp"

int main() {
  switch (7) {
  case 1: bouncing_spheres();  break;
  case 2: checkered_spheres(); break;
  case 3: earth(); break;
  case 4: perlin_spheres(); break;
  case 5: quads(); break;
  case 6: simple_light(); break;
  case 7: cornell_box(); break;
  case 8: cornell_smoke(); break;
  case 9: final_scene(400, 250, 4); break;
  case 10: final_scene(1920, 10000, 40); break;
  case 11: glass_pyr_test(); break;
  default: boosted_scene(800, 10000, 50); break;
  }
}