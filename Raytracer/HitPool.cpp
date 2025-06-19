#include "HitPool.hpp"

HitPool::HitPool(std::shared_ptr<Hittable> object) { add(object); }

void HitPool::clear() { hit_objects.clear(); }

void HitPool::add(std::shared_ptr<Hittable> object) {
  hit_objects.push_back(object);
}

bool HitPool::hit(const Ray& r, Interval interval, HitRecord& rec) const {
  HitRecord temp_rec;
  bool hit_anything = false;
  auto closest_so_far = interval.max;

  for (const std::shared_ptr<Hittable>& object : hit_objects) {
    if (object->hit(r, Interval(interval.min, closest_so_far), temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }

  return hit_anything;
}