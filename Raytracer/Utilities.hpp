#pragma once

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <glm/glm.hpp> // vec3, dot, normalize, ...

// Constants
inline const double infinity = std::numeric_limits<double>::infinity();
inline const double pi = 3.1415926535897932385;

// Utility Functions
inline double degrees_to_radians(double degrees) {
  return degrees * pi / 180.0;
}

inline double random_double() {
  // Returns a random real in [0, 1).
  return rand() / (RAND_MAX + 1.0);
}

inline double random_double(double min, double max) {
  // Returns a random real in [min, max).
  return min + (max - min) * random_double();
}

inline void write_color(std::ostream& out, const glm::vec3 color) {
  // Translate the [0, 1] component values to the byte range [0, 255].
  static const Interval intensity(0.000, 0.999);

  out << static_cast<int>(256 * intensity.clamp(color.r)) << ' '
      << static_cast<int>(256 * intensity.clamp(color.g)) << ' '
      << static_cast<int>(256 * intensity.clamp(color.b)) << '\n';
}

