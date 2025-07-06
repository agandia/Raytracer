#pragma once

#include "Constants.hpp"
#include "Interval.hpp"
#include <cstdlib>
#include <iostream>
#include <random>
#include <omp.h>
#include <glm/glm.hpp> // vec3, dot, normalize, ...
#include <glm/gtx/norm.hpp> // length2

// Utility ---------------------------------------------------------------------
inline double rnd()
{
  thread_local std::mt19937_64 rng(
    2025 +
#ifdef _OPENMP
    omp_get_thread_num()
#else
    0
#endif
  );
  thread_local std::uniform_real_distribution<double> uni(0.0, 1.0);
  return uni(rng);
}


// Utility Functions
inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

inline bool near_zero(const glm::dvec3& v) {
  // Returns true if the vector is close to zero in all components.
  const double s = 1e-8;
  return (fabs(v.x) < s) && (fabs(v.y) < s) && (fabs(v.z) < s);
}

inline double random_double() {
  // Returns a random real in [0, 1).
  return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
  // Returns a random real in [min, max).
  return min + (max - min) * random_double();
}

inline int random_int(int min, int max) {
  // Returns a random integer in [min, max).
  return int(random_double(min, max + 1));
}

inline static glm::dvec3 random() {
  return glm::dvec3(random_double(), random_double(), random_double());
}

inline static glm::dvec3 random(double min, double max) {
  return glm::dvec3(random_double(min, max), random_double(min, max), random_double(min, max));
}

inline glm::dvec3 random_unit_vector() {
  while (true) {
     glm::dvec3 p = random(-1, 1);
     double lengsq = glm::length2(p);
     if (1e-160 < lengsq && lengsq <= 1)
       return glm::normalize(p);
  }
}

inline glm::dvec3 random_in_unit_disk() {
  while (true) {
    glm::dvec3 p = glm::dvec3(random_double(-1.0, 1.0), random_double(-1.0, 1.0), 0.0);
    if (glm::length2(p) < 1.0)
      return p;
  }
}

inline glm::dvec2 sample_disk(double u1, double u2) {
  double r = std::sqrt(u1);
  double theta = 2.0 * pi * u2;
  return glm::dvec2(r * std::cos(theta), r * std::sin(theta));
}

inline glm::dvec3 random_on_hemisphere(const glm::dvec3& normal) {
  glm::dvec3 on_unit_sphere = random_unit_vector();
  if (glm::dot(on_unit_sphere, normal) > 0.0) { // In the same hemisphere as the normal
    return on_unit_sphere;
  } else {
    return -on_unit_sphere;
  }
}

inline glm::dvec3  random_cosine_direction() {
  double r1 = random_double();
  double r2 = random_double();

  double phi = 2 * pi * r1;
  double x = std::cos(phi) * std::sqrt(r2);
  double y = std::sin(phi) * std::sqrt(r2);
  double z = std::sqrt(1 - r2);

  return glm::dvec3(x, y, z);
}

inline glm::dvec3 reflect(const glm::dvec3& v, const glm::dvec3& n) {
  // Reflects vector v around normal n.
  return v - 2 * glm::dot(v, n) * n;
}

inline glm::dvec3 refract(const glm::dvec3& uv, const glm::dvec3& n, double etai_over_etat) {
  // Refracts vector uv through normal n with the given ratio of indices of refraction.
  double cos_theta = glm::min(glm::dot(-uv, n), 1.0);
  glm::dvec3 r_out_perp = etai_over_etat * (uv + cos_theta * n);
  glm::dvec3 r_out_parallel = -sqrt(fabs(1.0 - glm::length2(r_out_perp))) * n;
  return r_out_perp + r_out_parallel;
}

inline float linear_to_gamma(float value) {
  // Converts a linear color value to gamma space.
  if(value > 0.f)
    return std::sqrt(value);

  return 0.f;
}

inline void write_color(std::ostream& out, const glm::vec3 pixel_color) {
  float r = pixel_color.x;
  float g = pixel_color.y;
  float b = pixel_color.z;

  // Replace NaN components with zero.
  if (r != r) r = 0.0;
  if (g != g) g = 0.0;
  if (b != b) b = 0.0;

  // Apply a linear to gamma transform for gamma 2
  r = linear_to_gamma(r);
  g = linear_to_gamma(g);
  b = linear_to_gamma(b);

  // Translate the [0,1] component values to the byte range [0,255].
  static const Interval intensity(0.000, 0.999);
  int rbyte = int(256 * intensity.clamp(r));
  int gbyte = int(256 * intensity.clamp(g));
  int bbyte = int(256 * intensity.clamp(b));

  // Write out the pixel color components.
  out << rbyte << ' ' << gbyte << ' ' << bbyte << '\n';

}

