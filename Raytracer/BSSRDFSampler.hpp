#pragma once
#include "glm/glm.hpp"
#include "Constants.hpp"

class BSSRDFSampler {
public:
  double sigma_s;
  double sigma_a;
  double g;

  // Precomputed values:
  double sigma_s_prime; // reduced scattering
  double sigma_t_prime; // σ_s' + σ_a
  double D;
  double sigma_tr;
  double zr;
  double zv;

  BSSRDFSampler(double sigma_s_, double sigma_a_, double g_ = 0.0)
    : sigma_s(sigma_s_), sigma_a(sigma_a_), g(g_) {
    sigma_s_prime = sigma_s * (1.0 - g);
    sigma_t_prime = sigma_s_prime + sigma_a;
    D = 1.0 / (3.0 * sigma_t_prime);
    sigma_tr = std::sqrt(3.0 * sigma_a * sigma_t_prime);
    zr = 1.0 / sigma_t_prime;

    double A = 1.0; // Approximated; later you can derive A from internal Fresnel reflectance
    zv = zr + 4.0 * A * D;
  }

  // Diffuse reflectance profile R_d(r)
  double diffuse_profile(double r) const {
    double dr = std::sqrt(r * r + zr * zr);
    double dv = std::sqrt(r * r + zv * zv);

    double term_r = zr * (1.0 + sigma_tr * dr) * std::exp(-sigma_tr * dr) / (dr * dr * dr);
    double term_v = zv * (1.0 + sigma_tr * dv) * std::exp(-sigma_tr * dv) / (dv * dv * dv);

    return (term_r + term_v) / (4.0 * pi);
  }
};
