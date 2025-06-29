#include "Pyramid.hpp"

Pyramid::Pyramid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, std::shared_ptr<Material> m) {
  // returns the 3D pyramid that contains the two corners.
  faces = std::make_shared<HitPool>();

  // Area-weighted PDF initialization
  double total_area = 0.0;
  
  // Base quad (same as in box)
  std::shared_ptr<Quad> base = std::make_shared<Quad>(Q, u, v, m);
  double area_b = base->surface_area();
  total_area += area_b;
  faces->add(base);
  surface_areas.push_back(area_b); // Area from the base

  // Corners of base (winding counter-clockwise)
  glm::dvec3 b0 = Q;
  glm::dvec3 b1 = Q + u;
  glm::dvec3 b2 = Q + u + v;
  glm::dvec3 b3 = Q + v;

  // Apex point above center of base
  glm::dvec3 center = Q + 0.5 * u + 0.5 * v;
  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 apex = center + height * normal;

  std::shared_ptr<Triangle> side = std::make_shared<Triangle>(b0, b1 - b0, apex - b0, m);
  double area_s = side->surface_area(); 
  surface_areas.push_back(area_s);
  total_area += area_s;
  faces->add(side);

  side = std::make_shared<Triangle>(b1, b2 - b1, apex - b1, m);
  area_s = side->surface_area();
  surface_areas.push_back(area_s);
  total_area += area_s;
  faces->add(side);

  side = std::make_shared<Triangle>(b2, b3 - b2, apex - b2, m);
  area_s = side->surface_area();
  surface_areas.push_back(area_s);
  total_area += area_s;
  faces->add(side);

  side = std::make_shared<Triangle>(b3, b0 - b3, apex - b3, m);
  area_s = side->surface_area();
  surface_areas.push_back(area_s);
  total_area += area_s;
  faces->add(side);

  for (double& a : surface_areas) {
    a /= total_area;
  }

  bbox = faces->bounding_box();
}

bool Pyramid::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  return faces->hit(ray, ray_t, rec);
}

//double Pyramid::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
//  return faces->pdf_value(origin, direction);
//}
//
//glm::dvec3 Pyramid::random(const glm::dvec3& origin) const {
//  return faces->random(origin);
//}

AABB Pyramid::bounding_box() const {
  return faces->bounding_box();
}

double Pyramid::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  double pdf = 0.0;
  for (size_t i = 0; i < faces->hit_objects.size(); ++i) {
    pdf += surface_areas[i] * faces->hit_objects[i]->pdf_value(origin, direction);
  }
  return std::max(pdf, 1e-4); // Avoid numerical explosion from 1/pdf
}

glm::dvec3 Pyramid::random(const glm::dvec3& origin) const {
  double r = random_double();
  double cumulative = 0.0;

  for (size_t i = 0; i < faces->hit_objects.size(); ++i) {
    cumulative += surface_areas[i];
    if (r <= cumulative)
      return faces->hit_objects[i]->random(origin);
  }

  // Fallback (should not happen)
  return faces->hit_objects.back()->random(origin);
}