#include "HitPool.hpp"
#include "Utilities.hpp"

HitPool::HitPool(std::shared_ptr<Hittable> object) { add(object); }

void HitPool::clear() { hit_objects.clear(); }

void HitPool::add(std::shared_ptr<Hittable> object) {
  hit_objects.push_back(object);
  bbox = AABB(bbox, object->bounding_box());
}

bool HitPool::hit(const Ray& r, Interval interval, HitRecord& rec) const {
  HitRecord temp_rec;
  bool hit_anything = false;
  double closest_so_far = interval.max;

  for (const std::shared_ptr<Hittable>& object : hit_objects) {
    if (object->hit(r, Interval(interval.min, closest_so_far), temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }

  return hit_anything;
}

double HitPool::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  double weight = 1.0 / hit_objects.size();
  double sum = 0.0;
  for (const std::shared_ptr<Hittable>& object : hit_objects) {
    sum += weight * object->pdf_value(origin, direction);
  }
  return sum;
}

glm::dvec3 HitPool::random(const glm::dvec3& origin) const {
  int int_size = (int)hit_objects.size();
  return hit_objects[(random_int(0, int_size-1))]->random(origin);
}