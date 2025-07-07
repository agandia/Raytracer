#include "TextureWrapper.hpp"
#include "Interval.hpp"

glm::dvec3 CheckerTexture::color_value(double u, double v, const glm::dvec3& point) const {
  int xInteger = static_cast<int>(std::floor(inverse_scale * point.x));
  int yInteger = static_cast<int>(std::floor(inverse_scale * point.y));
  int zInteger = static_cast<int>(std::floor(inverse_scale * point.z));

  bool isEven = (xInteger + yInteger + zInteger) % 2 == 0;

  return isEven ? even->color_value(u, v, point) : odd->color_value(u, v, point);
}

glm::dvec3 ImageTexture::color_value(double u, double v, const glm::dvec3& /*point*/) const {
  // If we have no texture data, return solid cyan as a noticeable error color
  if (image.height() <= 0) return glm::dvec3(0, 1, 1);
  
  // Clamp input texture coordinates to [0,1] x [1,0]
  u = Interval(0, 1).clamp(u);
  v = 1.0 - Interval(0, 1).clamp(v); // Flip v to match image coordinates

  int i = static_cast<int>(u * image.width());
  int j = static_cast<int>(v * image.height());
  const unsigned char * pixel = image.pixel_data(i,j);

  double color_scale = 1.0/ 255.0;

  return glm::dvec3(color_scale*pixel[0], color_scale*pixel[1], color_scale*pixel[2]);
}
