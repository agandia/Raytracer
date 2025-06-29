#include "Cone.hpp"

#include "Cone.hpp"

Cone::Cone(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m)
{
  sides = std::make_shared<HitPool>();

  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 apex = Q + normal * height;

  // Base ellipse
  auto base = std::make_shared<Ellipse>(Q, u, v, m);
  sides->add(base);
  double total_area = base->surface_area();
  surface_areas.push_back(total_area);

  // Side triangles
  for (int i = 0; i < segments; ++i) {
    double a0 = i * 2.0 * pi / segments;
    double a1 = (i + 1) * 2.0 * pi / segments;

    glm::dvec3 p0 = Q + cos(a0) * u + sin(a0) * v;
    glm::dvec3 p1 = Q + cos(a1) * u + sin(a1) * v;

    auto tri = std::make_shared<Triangle>(p0, p1 - p0, apex - p0, m);
    sides->add(tri);
    double tri_area = tri->surface_area();
    surface_areas.push_back(tri_area);
    total_area += tri_area;
  }

  // Normalize weights
  for (double& area : surface_areas) {
    area /= total_area;
  }

  bbox = sides->bounding_box();
}

double Cone::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  double pdf = 0.0;
  for (size_t i = 0; i < sides->hit_objects.size(); ++i) {
    pdf += surface_areas[i] * sides->hit_objects[i]->pdf_value(origin, direction);
  }
  return std::max(pdf, 1e-4);
}

glm::dvec3 Cone::random(const glm::dvec3& origin) const {
  double r = random_double();
  double cumulative = 0.0;

  for (size_t i = 0; i < sides->hit_objects.size(); ++i) {
    cumulative += surface_areas[i];
    if (r <= cumulative)
      return sides->hit_objects[i]->random(origin);
  }

  // Fallback (should not occur)
  return sides->hit_objects.back()->random(origin);
}

AABB Cone::bounding_box() const {
  return bbox;
}

bool Cone::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  return sides->hit(ray, ray_t, rec);
}
