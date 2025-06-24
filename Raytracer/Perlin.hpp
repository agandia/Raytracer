#pragma once

#include <glm/glm.hpp>

class PerlinNoise {
  public:
    PerlinNoise();
    // Generate 3D Perlin noise
    double noise(const glm::dvec3& point) const;

  private:
    static const int point_count = 256;
    double rand_double[point_count];
    int perm_x[point_count];
    int perm_y[point_count];
    int perm_z[point_count];

    static void perlin_generate_permutation(int* perm);
    static void permute(int* perm, int size);
};