#include "Scene.hpp"

int main() {
  switch (5) {
  case 1: bouncing_spheres();  break;
  case 2: checkered_spheres(); break;
  case 3: earth(); break;
  case 4: perlin_spheres(); break;
  case 5: quads(); break;
  }
}