// Cylindroid.cpp
#include "Cylindroid.hpp"


Cylindroid::Cylindroid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m) {
  sides = std::make_shared<HitPool>();
  double total_area = 0.0;

  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 Q_top = Q + normal * height;

  // Base and top
  auto base = std::make_shared<Ellipse>(Q, u, v, m);
  auto top = std::make_shared<Ellipse>(Q_top, u, v, m);
  sides->add(base);
  sides->add(top);
  surface_areas.push_back(base->surface_area());
  surface_areas.push_back(top->surface_area());
  total_area += surface_areas[0] + surface_areas[1];

  // Side walls
  for (int i = 0; i < segments; ++i) {
    double a0 = (i + 0) * 2.0 * pi / segments;
    double a1 = (i + 1) * 2.0 * pi / segments;

    glm::dvec3 p0 = Q + cos(a0) * u + sin(a0) * v;
    glm::dvec3 p1 = Q + cos(a1) * u + sin(a1) * v;

    glm::dvec3 p0_top = p0 + normal * height;
    glm::dvec3 p1_top = p1 + normal * height;

    auto quad = std::make_shared<Quad>(p0, p1 - p0, p0_top - p0, m);
    sides->add(quad);

    double area = quad->surface_area();
    surface_areas.push_back(area);
    total_area += area;
  }

  // Normalize to PDF weights
  for (double& area : surface_areas)
    area /= total_area;

  bbox = sides->bounding_box();
}

bool Cylindroid::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  return sides->hit(ray, ray_t, rec);
}

double Cylindroid::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  double pdf = 0.0;
  for (size_t i = 0; i < sides->hit_objects.size(); ++i) {
    pdf += surface_areas[i] * sides->hit_objects[i]->pdf_value(origin, direction);
  }
  return std::max(pdf, 1e-4);
}

glm::dvec3 Cylindroid::random(const glm::dvec3& origin) const {
  double r = random_double();
  double cumulative = 0.0;

  for (size_t i = 0; i < sides->hit_objects.size(); ++i) {
    cumulative += surface_areas[i];
    if (r <= cumulative)
      return sides->hit_objects[i]->random(origin);
  }

  // Fallback (should not happen)
  return sides->hit_objects.back()->random(origin);
}

AABB Cylindroid::bounding_box() const {
  return sides->bounding_box();
}
