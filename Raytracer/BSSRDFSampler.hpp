#pragma once
#include <vector>
#include <cmath>
#include <algorithm>
#include "glm/glm.hpp"
#include "Constants.hpp"

class BSSRDFSampler {
public:
  // Optical parameters
  double sigma_s;         // Scattering coefficient
  double sigma_a;         // Absorption coefficient
  double sigma_s_prime;   // Reduced scattering coefficient (σ_s' = σ_s * (1 - g))
  double sigma_t_prime;   // σ_t' = σ_s' + σ_a
  double D;               // Diffusion coefficient
  double sigma_tr;        // Effective attenuation coefficient
  double zr;              // Real source depth
  double zv;              // Virtual source depth

  // CDF sampling
  std::vector<double> cdf;
  std::vector<double> radii;
  double R_max = 100.0;

  BSSRDFSampler(double sigma_s_, double sigma_a_, double g_ = 0.0)
    : sigma_s(sigma_s_), sigma_a(sigma_a_) {
    sigma_s_prime = sigma_s * (1.0 - g_);
    sigma_t_prime = sigma_s_prime + sigma_a;
    D = 1.0 / (3.0 * sigma_t_prime);
    sigma_tr = std::sqrt(3.0 * sigma_a * sigma_t_prime);
    zr = 1.0 / sigma_t_prime;

    double A = 1.0; // Approximate for now, real model uses Fresnel reflectance
    zv = zr + 4.0 * A * D;

    precompute_cdf();
  }

  // Dipole diffuse profile R_d(r)
  double diffuse_profile(double r) const {
    double dr = std::sqrt(r * r + zr * zr);
    double dv = std::sqrt(r * r + zv * zv);

    double term_r = zr * (1.0 + sigma_tr * dr) * std::exp(-sigma_tr * dr) / (dr * dr * dr);
    double term_v = zv * (1.0 + sigma_tr * dv) * std::exp(-sigma_tr * dv) / (dv * dv * dv);

    return (term_r + term_v) / (4.0 * pi);
  }

  // Precompute CDF for sampling radius
  void precompute_cdf() {
    const int N = 512;
    cdf.resize(N);
    radii.resize(N);

    double dr = R_max / (N - 1);
    double accum = 0.0;

    for (int i = 0; i < N; ++i) {
      double r = i * dr;
      double profile = diffuse_profile(r);
      double shell_area = 2.0 * pi * r * dr;
      double weighted = profile * shell_area;
      accum += weighted;

      radii[i] = r;
      cdf[i] = accum;
    }

    // Normalize CDF to [0,1]
    for (int i = 0; i < N; ++i) {
      cdf[i] /= accum;
    }
  }

  // Inverse CDF sampling of radius
  double sample_radius(double u) const {
    auto it = std::lower_bound(cdf.begin(), cdf.end(), u);
    int idx = std::clamp(int(it - cdf.begin()), 1, int(cdf.size() - 1));

    // Linear interpolation
    double t = (u - cdf[idx - 1]) / (cdf[idx] - cdf[idx - 1]);
    return glm::mix(radii[idx - 1], radii[idx], t);
  }
};
