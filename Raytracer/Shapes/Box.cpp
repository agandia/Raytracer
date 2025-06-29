#include "Box.hpp"

Box::Box(const glm::dvec3& corner_a, const glm::dvec3& corner_b, std::shared_ptr<Material> m) {
  sides = std::make_shared<HitPool>();

  glm::dvec3 min_corner = glm::min(corner_a, corner_b);
  glm::dvec3 max_corner = glm::max(corner_a, corner_b);

  glm::dvec3 dx = glm::dvec3(max_corner.x - min_corner.x, 0, 0);
  glm::dvec3 dy = glm::dvec3(0, max_corner.y - min_corner.y, 0);
  glm::dvec3 dz = glm::dvec3(0, 0, max_corner.z - min_corner.z);

  sides->add(std::make_shared<Quad>(min_corner + dz, dx, dy, m));  // Front
  sides->add(std::make_shared<Quad>(min_corner + dx + dz, -dz, dy, m)); // Right
  sides->add(std::make_shared<Quad>(min_corner + dx, -dx, dy, m)); // Back
  sides->add(std::make_shared<Quad>(min_corner, dz, dy, m));       // Left
  sides->add(std::make_shared<Quad>(max_corner, -dx, -dz, m));     // Top
  sides->add(std::make_shared<Quad>(min_corner, dx, dz, m));       // Bottom

  // Area-weighted PDF initialization
  double total_area = 0.0;
  for (int i = 0; i < 6; ++i) {
    face_weights[i] = sides->hit_objects[i]->bounding_box().surface_area(); // Approx area from AABB
    total_area += face_weights[i];
  }
  for (int i = 0; i < 6; ++i) {
    face_weights[i] /= total_area; // Normalize to PDF weights
  }

  bbox = AABB(min_corner, max_corner);
}

bool Box::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  return sides->hit(ray, ray_t, rec);
}

double Box::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  
  double pdf = 0.0;
  for (int i = 0; i < 6; ++i) {
    pdf += face_weights[i] * sides->hit_objects[i]->pdf_value(origin, direction);
  }
  return pdf;
}


glm::dvec3 Box::random(const glm::dvec3& origin) const {
  
  double r = random_double();
  double cumulative = 0.0;

  for (int i = 0; i < 6; ++i) {
    cumulative += face_weights[i];
    if (r <= cumulative) {
      return sides->hit_objects[i]->random(origin);
    }
  }
  // fallback, shouldn't happen
  return sides->hit_objects[5]->random(origin);

}

AABB Box::bounding_box() const {
  return sides->bounding_box();
}