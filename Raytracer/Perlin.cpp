#include "Perlin.hpp"
#include "Utilities.hpp"
#include <utility>

PerlinNoise::PerlinNoise()
{
  for(int i = 0; i < point_count; i++) {
    rand_vec[i] = glm::normalize(random(-1.0, 1.0));
  }

  perlin_generate_permutation(perm_x);
  perlin_generate_permutation(perm_y);
  perlin_generate_permutation(perm_z);
}

double PerlinNoise::noise(const glm::dvec3& point) const
{
  const double u = point.x - std::floor(point.x);
  const double v = point.y - std::floor(point.y);
  const double w = point.z - std::floor(point.z);

  const int i = int(std::floor(point.x));
  const int j = int(std::floor(point.y));
  const int k = int(std::floor(point.z));

  glm::dvec3 c[2][2][2]; // Interpolating dimensional cube

  for (int di = 0; di < 2; di++)
    for (int dj = 0; dj < 2; dj++)
      for (int dk = 0; dk < 2; dk++)
        c[di][dj][dk] = rand_vec[
          perm_x[(i + di) & 255] ^
            perm_y[(j + dj) & 255] ^
            perm_z[(k + dk) & 255]
        ];

  return perlin_interpolate(c, u, v, w);
}

// Composite noise using multiple octaves or frequencies of noise added together
double PerlinNoise::turbulence(const glm::dvec3& point, int octaves) const
{
  double accum = 0.0;
  glm::dvec3 temp_point = point; //modifyable copy
  double weight = 1.0;

  for(int i = 0; i < octaves; i++) {
    accum += weight * noise(temp_point);
    weight *= 0.5; // Halve the weight for each octave
    temp_point *= 2.0; // Double the frequency
  }
  return glm::abs(accum);
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

double PerlinNoise::perlin_interpolate(const glm::dvec3 (&c)[2][2][2], double u, double v, double w)
{
  //Smooth Mach bands with Hermitian smoothing
  const double uu = u * u * (3 - 2 * u);
  const double vv = v * v * (3 - 2 * v);
  const double ww = w * w * (3 - 2 * w);

  double accum = 0.0;
  for (int i = 0; i < 2; i++)
    for (int j = 0; j < 2; j++)
      for (int k = 0; k < 2; k++) {
        glm::dvec3 weight_v(u-i, v-j, k-w);
        accum += (i * uu + (1 - i) * (1 - uu))
          * (j * vv + (1 - j) * (1 - vv))
          * (k * ww + (1 - k) * (1 - ww))
          * glm::dot(c[i][j][k], weight_v);
      }

        
  return accum;
}

