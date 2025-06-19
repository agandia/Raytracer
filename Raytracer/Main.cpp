#include <iostream>

int main() {
  // Image
  int image_width = 256;
  int image_height = 256;

  //Render
  std::cout << "P3\n" << image_width << " " << image_height << "\n255\n";

  for (int j = 0; j < image_height; j++) {
    for (int i = 0; i < image_width; i++) {
      float r = float(i) / (image_width - 1);
      float g = float(j) / (image_height - 1);
      float b = 0.0f;

      int ir = int(255.99f * r);
      int ig = int(255.99f * g);
      int ib = int(255.99f * b);

      std::cout << ir << " " << ig << " " << ib << "\n";
    }
  }
}