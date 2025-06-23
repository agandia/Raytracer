#pragma once

#include <memory>
#include <glm/glm.hpp>
#include "ITexture.hpp"
#include "ImageLoader.hpp"

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