#include "ConstantMedium.hpp"

bool ConstantMedium::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
  HitRecord rec1, rec2;

  if (!boundary->hit(r, Interval::universe, rec1))
    return false;

  if (!boundary->hit(r, Interval(rec1.t + 0.0001, infinity), rec2))
    return false;

  if (rec1.t < ray_t.min) rec1.t = ray_t.min;
  if (rec2.t > ray_t.max) rec2.t = ray_t.max;

  if (rec1.t >= rec2.t)
    return false;

  if (rec1.t < 0)
    rec1.t = 0;

  double ray_length = glm::length(r.direction());
  double distance_inside_boundary = (rec2.t - rec1.t) * ray_length;
  double hit_distance = neg_inv_density * std::log(random_double());

  if (hit_distance > distance_inside_boundary)
    return false;

  rec.t = rec1.t + hit_distance / ray_length;
  rec.p = r.at(rec.t);

  rec.normal = glm::dvec3(1, 0, 0);  // arbitrary
  rec.front_face = true;     // also arbitrary
  rec.material = phase_function;

  return true;
}