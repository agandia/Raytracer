#include "Cone.hpp"

Cone::Cone(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m) {
  sides = std::make_shared<HitPool>();

  // Ellipse normal and apex
  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 apex = Q + normal * height;

  // Base ellipse
  sides->add(std::make_shared<Ellipse>(Q, u, v, m));

  // Side triangles
  int steps = segments;
  for (int i = 0; i < steps; ++i) {
    double a0 = (i + 0) * 2.0 * pi / steps;
    double a1 = (i + 1) * 2.0 * pi / steps;

    glm::dvec3 p0 = Q + cos(a0) * u + sin(a0) * v;
    glm::dvec3 p1 = Q + cos(a1) * u + sin(a1) * v;

    sides->add(std::make_shared<Triangle>(p0, p1 - p0, apex - p0, m));
  }
}

bool Cone::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  return sides->hit(ray, ray_t, rec);
}

double Cone::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  return sides->pdf_value(origin, direction);
}

glm::dvec3 Cone::random(const glm::dvec3& origin) const {
  return sides->random(origin);
}

AABB Cone::bounding_box() const {
  return sides->bounding_box();
}