#include "Cone.hpp"

Cone::Cone(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m)
{
  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 apex = Q + normal * height;

  base_Q_ = Q;
  axis_w_ = glm::normalize(normal);
  h_ = height;
  r_u_ = glm::length(u);
  r_v_ = glm::length(v);
  b_u_ = u;
  b_v_ = v;

  // Base ellipse
  auto base = std::make_shared<Ellipse>(Q, u, v, m);
  face_list.push_back(base);
  double total_area = base->surface_area();
  surface_areas.push_back(total_area);

  // Side triangles
  for (int i = 0; i < segments; ++i) {
    double a0 = i * 2.0 * pi / segments;
    double a1 = (i + 1) * 2.0 * pi / segments;

    glm::dvec3 p0 = Q + cos(a0) * u + sin(a0) * v;
    glm::dvec3 p1 = Q + cos(a1) * u + sin(a1) * v;

    auto tri = std::make_shared<Triangle>(p0, p1 - p0, apex - p0, m);
    face_list.push_back(tri);
    double tri_area = tri->surface_area();
    surface_areas.push_back(tri_area);
    total_area += tri_area;
  }

  // Normalize weights
  for (double& area : surface_areas) {
    area /= total_area;
  }

  sides_bvh = std::make_shared<BVHNode>(face_list, 0, face_list.size());
  bbox = sides_bvh->bounding_box();
}

bool Cone::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  bool hit_side = sides_bvh->hit(ray, ray_t, rec);
  if (hit_side) rec.shape_ptr = this;
  return hit_side;
}

bool Cone::contains(const glm::dvec3& p) const
{
  // axial coordinate
  double t = glm::dot(p - base_Q_, axis_w_);
  if (t < 1e-6 || t > h_ - 1e-6) return false;  // outside height

  // project onto base plane
  glm::dvec3 q = p - axis_w_ * t;
  glm::dvec3 rel = q - base_Q_;

  double a = glm::dot(rel, glm::normalize(b_u_)) / r_u_;
  double b = glm::dot(rel, glm::normalize(b_v_)) / r_v_;

  // radius shrinks linearly toward apex
  double s = (h_ - t) / h_;     // 1 at base → 0 at apex
  return (a * a + b * b) <= s * s + 1e-6;
}

AABB Cone::bounding_box() const {
  return bbox;
}

double Cone::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  double pdf = 0.0;
  for (size_t i = 0; i < face_list.size(); ++i) {
    pdf += surface_areas[i] * face_list[i]->pdf_value(origin, direction);
  }
  return std::max(pdf, 1e-6);
}

glm::dvec3 Cone::random(const glm::dvec3& origin) const {
  double r = random_double();
  double cumulative = 0.0;

  for (size_t i = 0; i < face_list.size(); ++i) {
    cumulative += surface_areas[i];
    if (r <= cumulative)
      return face_list[i]->random(origin);
  }

  // Fallback (should not occur)
  return face_list.back()->random(origin);
}
