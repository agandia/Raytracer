#pragma once

#include <glm/glm.hpp>

class PerlinNoise {
  public:
    PerlinNoise();
    // Generate 3D Perlin noise
    double noise(const glm::dvec3& point) const;
    double turbulence(const glm::dvec3& point, int octaves = 1) const;

  private:
    static const int point_count = 256;
    glm::dvec3 rand_vec[point_count]; //use vectors instead of doubles to avoid blocky looks at latice points
    int perm_x[point_count];
    int perm_y[point_count];
    int perm_z[point_count];

    static void perlin_generate_permutation(int* perm);
    static void permute(int* perm, int size);
    static double perlin_interpolate(const glm::dvec3(&c)[2][2][2], double x, double y, double z);
};