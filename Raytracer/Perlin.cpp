#include "Perlin.hpp"
#include "Utilities.hpp"
#include <utility>

PerlinNoise::PerlinNoise()
{
  for(int i = 0; i < point_count; i++) {
    rand_double[i] = random_double();
  }

  perlin_generate_permutation(perm_x);
  perlin_generate_permutation(perm_y);
  perlin_generate_permutation(perm_z);
}

double PerlinNoise::noise(const glm::dvec3& point) const
{
  const int i = int(4 * point.x) & 255;
  const int j = int(4 * point.y) & 255;
  const int k = int(4 * point.z) & 255;

  return rand_double[perm_x[i] ^ perm_y[j] ^ perm_z[k]];
}

void PerlinNoise::perlin_generate_permutation(int* perm)
{
  for(int i = 0; i < point_count; i++) {
    perm[i] = i;
  }
  permute(perm, point_count);
}

void PerlinNoise::permute(int* perm, int size)
{
  for(int i = size - 1; i > 0; i--) {
    int target = random_int(0, i);
    std::swap(perm[i], perm[target]);
  }
}

