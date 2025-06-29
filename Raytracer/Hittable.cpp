#include "Hittable.hpp"

bool Translate::hit(const Ray& r, Interval t, HitRecord& rec) const {
  // Move the ray backwards by the offset
  Ray offset_r(r.origin() - offset, r.direction(), r.time());

  // Determine whether an intersection exists along the offset ray (and if so, where)
  if (!hittable->hit(offset_r, t, rec))
    return false;

  // Move the intersection point forwards by the offset
  rec.p += offset;

  return true;
}


RotateYAxis::RotateYAxis(std::shared_ptr<Hittable> object, double angle)
  : hittable(object) {
  double radians = glm::radians(angle);
  sin_theta = sin(radians);
  cos_theta = cos(radians);

  bbox = hittable->bounding_box();

  glm::dvec3 min_point(infinity, infinity, infinity);
  glm::dvec3 max_point(-infinity, -infinity, -infinity);

  for (int i = 0; i < 2; ++i) {
    for (int j = 0; j < 2; ++j) {
      for (int k = 0; k < 2; ++k) {
        double x = i * bbox.x.max + (1 - i) * bbox.x.min;
        double y = j * bbox.y.max + (1 - j) * bbox.y.min;
        double z = k * bbox.z.max + (1 - k) * bbox.z.min;

        double new_x = cos_theta * x + sin_theta * z;
        double new_z = -sin_theta * x + cos_theta * z;

        glm::dvec3 tester(new_x, y, new_z);

        for (int c = 0; c < 3; c++) {
          min_point[c] = std::fmin(min_point[c], tester[c]);
          max_point[c] = std::fmax(max_point[c], tester[c]);
        }
      }
    }
  }

  bbox = AABB(min_point, max_point);
}

bool RotateYAxis::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {

  // Transform the ray from world space to object space.

  auto origin = glm::dvec3((cos_theta * r.origin().x) - (sin_theta * r.origin().z), r.origin().y, (sin_theta * r.origin().x) + (cos_theta * r.origin().z));

  auto direction = glm::dvec3((cos_theta * r.direction().x) - (sin_theta * r.direction().z), r.direction().y, (sin_theta * r.direction().x) + (cos_theta * r.direction().z));

  Ray rotated_r(origin, direction, r.time());

  // Determine whether an intersection exists in object space (and if so, where).

  if (!hittable->hit(rotated_r, ray_t, rec))
    return false;

  // Transform the intersection from object space back to world space.

  rec.p = glm::dvec3((cos_theta * rec.p.x) + (sin_theta * rec.p.z), rec.p.y, (-sin_theta * rec.p.x) + (cos_theta * rec.p.z));

  rec.normal = glm::dvec3((cos_theta * rec.normal.x) + (sin_theta * rec.normal.z), rec.normal.y, (-sin_theta * rec.normal.x) + (cos_theta * rec.normal.z));

  return true;
}

glm::dvec3 RotateYAxis::rotate_y(const glm::dvec3& p) const {
  return glm::dvec3(
    cos_theta * p.x + sin_theta * p.z,
    p.y,
    -sin_theta * p.x + cos_theta * p.z
  );
}

glm::dvec3 RotateYAxis::inverse_rotate_y(const glm::dvec3& p) const {
  return glm::dvec3(
    cos_theta * p.x - sin_theta * p.z,
    p.y,
    sin_theta * p.x + cos_theta * p.z
  );
}

double RotateYAxis::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  glm::dvec3 origin_local = inverse_rotate_y(origin);
  glm::dvec3 direction_local = inverse_rotate_y(direction);
  return hittable->pdf_value(origin_local, direction_local);
}

glm::dvec3 RotateYAxis::random(const glm::dvec3& origin) const {
  glm::dvec3 origin_local = inverse_rotate_y(origin);
  glm::dvec3 random_local = hittable->random(origin_local);
  return rotate_y(random_local);
}
