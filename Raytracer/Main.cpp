#include <iostream>
#include "glm/glm.hpp"
#include "Utilities.hpp"

int main() {
  // Image
  int image_width = 256;
  int image_height = 256;

  //Render
  std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

  for (int j = 0; j < image_height; j++) {
    std::clog << "\rScanlines remaining: " << image_height - j << ' ' << std::flush;
    for (int i = 0; i < image_width; i++) {
      const glm::vec3 color = glm::vec3(double(i) / (image_width - 1), double(j) / (image_height - 1), 0.0f);
      write_color(std::cout, color);
    }
  }
  std::clog << "\rDone.                   \n";
}