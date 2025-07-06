#include "Box.hpp"
#include <vector>

Box::Box(const glm::dvec3& corner_a, const glm::dvec3& corner_b, std::shared_ptr<Material> m) {
  
  glm::dvec3 min_corner = glm::min(corner_a, corner_b);
  glm::dvec3 max_corner = glm::max(corner_a, corner_b);

  glm::dvec3 dx = glm::dvec3(max_corner.x - min_corner.x, 0, 0);
  glm::dvec3 dy = glm::dvec3(0, max_corner.y - min_corner.y, 0);
  glm::dvec3 dz = glm::dvec3(0, 0, max_corner.z - min_corner.z);

  face_list.push_back(std::make_shared<Quad>(min_corner + dz, dx, dy, m));  // Front
  face_list.push_back(std::make_shared<Quad>(min_corner + dx + dz, -dz, dy, m)); // Right
  face_list.push_back(std::make_shared<Quad>(min_corner + dx, -dx, dy, m)); // Back
  face_list.push_back(std::make_shared<Quad>(min_corner, dz, dy, m));       // Left
  face_list.push_back(std::make_shared<Quad>(max_corner, -dx, -dz, m));     // Top
  face_list.push_back(std::make_shared<Quad>(min_corner, dx, dz, m));       // Bottom

  // Area-weighted PDF initialization
  double total_area = 0.0;
  for (int i = 0; i < 6; ++i) {
    face_weights[i] = face_list[i]->bounding_box().surface_area(); // Approx area from AABB
    total_area += face_weights[i];
  }
  for (int i = 0; i < 6; ++i) {
    face_weights[i] /= total_area; // Normalize to PDF weights
  }

  sides_bvh = std::make_shared<BVHNode>(face_list, 0, face_list.size());
  bbox = AABB(min_corner, max_corner);
}

bool Box::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  bool hit_side = sides_bvh->hit(ray, ray_t, rec);
  if ( hit_side) rec.shape_ptr = this;
  return hit_side;
}

bool Box::contains(const glm::dvec3& p) const {
  const double eps = 1e-6;
  return (p.x > bbox.x.min + eps && p.x < bbox.x.max - eps) &&
    (p.y > bbox.y.min + eps && p.y < bbox.y.max - eps) &&
    (p.z > bbox.z.min + eps && p.z < bbox.z.max - eps);
}

AABB Box::bounding_box() const {
  return sides_bvh->bounding_box();
}

double Box::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  
  double pdf = 0.0;
  for (int i = 0; i < 6; ++i) {
    pdf += face_weights[i] * face_list[i]->pdf_value(origin, direction);
  }
  return pdf;
}

glm::dvec3 Box::random(const glm::dvec3& origin) const {
  
  double r = random_double();
  double cumulative = 0.0;

  for (int i = 0; i < 6; ++i) {
    cumulative += face_weights[i];
    if (r <= cumulative) {
      return face_list[i]->random(origin);
    }
  }
  // fallback, shouldn't happen
  return face_list[5]->random(origin);
}