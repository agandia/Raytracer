#include "Pyramid.hpp"

Pyramid::Pyramid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, std::shared_ptr<Material> m) {
  // returns the 3D pyramid that contains the two corners.
  faces = std::make_shared<HitPool>();

  // Base quad (same as in box)
  faces->add(std::make_shared<Quad>(Q, u, v, m));

  // Corners of base (winding counter-clockwise)
  glm::dvec3 b0 = Q;
  glm::dvec3 b1 = Q + u;
  glm::dvec3 b2 = Q + u + v;
  glm::dvec3 b3 = Q + v;

  // Apex point above center of base
  glm::dvec3 center = Q + 0.5 * u + 0.5 * v;
  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 apex = center + height * normal;

  faces->add(std::make_shared<Triangle>(b0, b1 - b0, apex - b0, m));
  faces->add(std::make_shared<Triangle>(b1, b2 - b1, apex - b1, m));
  faces->add(std::make_shared<Triangle>(b2, b3 - b2, apex - b2, m));
  faces->add(std::make_shared<Triangle>(b3, b0 - b3, apex - b3, m));
}

bool Pyramid::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  return faces->hit(ray, ray_t, rec);
}

double Pyramid::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  return faces->pdf_value(origin, direction);
}

glm::dvec3 Pyramid::random(const glm::dvec3& origin) const {
  return faces->random(origin);
}

AABB Pyramid::bounding_box() const {
  return faces->bounding_box();
}