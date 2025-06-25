#include "Scene.hpp"

int main() {
  switch (6) {
  case 1: bouncing_spheres();  break;
  case 2: checkered_spheres(); break;
  case 3: earth(); break;
  case 4: perlin_spheres(); break;
  case 5: quads(); break;
  case 6: simple_light(); break;
  }
}