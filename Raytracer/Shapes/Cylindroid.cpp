// Cylindroid.cpp
#include "Cylindroid.hpp"


Cylindroid::Cylindroid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m) {
  sides = std::make_shared<HitPool>();

  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 Q_top = Q + normal * height;

  // Base and top
  sides->add(std::make_shared<Ellipse>(Q, u, v, m));
  sides->add(std::make_shared<Ellipse>(Q_top, u, v, m));

  // Side walls
  int steps = segments;
  for (int i = 0; i < steps; ++i) {
    double a0 = (i + 0) * 2.0 * pi / steps;
    double a1 = (i + 1) * 2.0 * pi / steps;

    glm::dvec3 p0 = Q + cos(a0) * u + sin(a0) * v;
    glm::dvec3 p1 = Q + cos(a1) * u + sin(a1) * v;

    glm::dvec3 p0_top = p0 + normal * height;
    glm::dvec3 p1_top = p1 + normal * height;

    sides->add(std::make_shared<Quad>(p0, p1 - p0, p0_top - p0, m));
  }
}

bool Cylindroid::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  return sides->hit(ray, ray_t, rec);
}

double Cylindroid::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  return sides->pdf_value(origin, direction);
}

glm::dvec3 Cylindroid::random(const glm::dvec3& origin) const {
  return sides->random(origin);
}

AABB Cylindroid::bounding_box() const {
  return sides->bounding_box();
}
