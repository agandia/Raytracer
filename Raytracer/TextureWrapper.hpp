#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "ITexture.hpp"
#include "ImageLoader.hpp"
#include "Perlin.hpp"


class SolidColorTexture : public ITexture {

public:
  SolidColorTexture(const glm::dvec3& albedo) : albedo_(albedo) {}
  SolidColorTexture(double r, double g, double b) : SolidColorTexture(glm::dvec3(r, g, b)) {}

  glm::dvec3 color_value(double u, double v, const glm::dvec3& point) const override {
    return albedo_;
  }

private:
  glm::dvec3 albedo_;
};

class CheckerTexture : public ITexture {
public:
  CheckerTexture(double scale, std::shared_ptr<ITexture> even, std::shared_ptr<ITexture> odd)
    : inverse_scale(1.0 / scale), even(even), odd(odd) {}

  CheckerTexture(double scale, const glm::dvec3& color1, const glm::dvec3& color2)
    : CheckerTexture(scale, std::make_shared<SolidColorTexture>(color1), std::make_shared<SolidColorTexture>(color2)) { }

  glm::dvec3 color_value(double u, double v, const glm::dvec3& point) const override;
private:
  double inverse_scale;
  std::shared_ptr<ITexture> even;
  std::shared_ptr<ITexture> odd;
};

class ImageTexture : public ITexture {
  public:
    ImageTexture(const char * filename) : image(filename) {};
    glm::dvec3 color_value(double u, double v, const glm::dvec3& point) const override;

  private:
    Image image;
};

class NoiseTexture : public ITexture {
  public:
    NoiseTexture(double scale) : scale(scale) {}

    glm::dvec3 color_value(double u, double v, const glm::dvec3& point) const override {
      return glm::dvec3(0.5) * (1.0 + glm::sin(scale * point.z + 10.0 * noise.turbulence(7.0 * point))); // Make sure we don't end up with negative values by shifting the noise output from [-1, 1] to [0, 1]
    }

  private:
    PerlinNoise noise;
    double scale;
};