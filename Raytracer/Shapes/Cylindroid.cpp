#include "Cylindroid.hpp"

Cylindroid::Cylindroid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m) {
  //sides = std::make_shared<HitPool>();
  double total_area = 0.0;

  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 Q_top = Q + normal * height;

  base_Q_ = Q;
  axis_w_ = glm::normalize(normal);
  h_ = height;
  r_u_ = glm::length(u);
  r_v_ = glm::length(v);
  b_u_ = u;
  b_v_ = v;

  // Base and top
  auto base = std::make_shared<Ellipse>(Q, u, v, m);
  auto top = std::make_shared<Ellipse>(Q_top, u, v, m);
  face_list.push_back(base);
  face_list.push_back(top);
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
    face_list.push_back(quad);

    double area = quad->surface_area();
    surface_areas.push_back(area);
    total_area += area;
  }

  // Normalize to PDF weights
  for (double& area : surface_areas)
    area /= total_area;

  sides_bvh = std::make_shared<BVHNode>(face_list, 0, face_list.size());
  bbox = sides_bvh->bounding_box();
}

bool Cylindroid::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  bool hit_side = sides_bvh->hit(ray, ray_t, rec);
  if (hit_side) rec.shape_ptr = this;
  return hit_side;
}

bool Cylindroid::contains(const glm::dvec3& p) const
{
  double t = glm::dot(p - base_Q_, axis_w_);
  if (t < 1e-6 || t > h_ - 1e-6) return false;

  glm::dvec3 q = p - axis_w_ * t;
  glm::dvec3 rel = q - base_Q_;

  double a = glm::dot(rel, glm::normalize(b_u_)) / r_u_;
  double b = glm::dot(rel, glm::normalize(b_v_)) / r_v_;

  return (a * a + b * b) <= 1.0 + 1e-6;
}


double Cylindroid::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  double pdf = 0.0;
  for (size_t i = 0; i < face_list.size(); ++i) {
    pdf += surface_areas[i] * face_list[i]->pdf_value(origin, direction);
  }
  return std::max(pdf, 1e-6);
}

glm::dvec3 Cylindroid::random(const glm::dvec3& origin) const {
  double r = random_double();
  double cumulative = 0.0;

  for (size_t i = 0; i < face_list.size(); ++i) {
    cumulative += surface_areas[i];
    if (r <= cumulative)
      return face_list[i]->random(origin);
  }

  // Fallback (should not happen)
  return face_list.back()->random(origin);
}

AABB Cylindroid::bounding_box() const {
  return sides_bvh->bounding_box();
}
