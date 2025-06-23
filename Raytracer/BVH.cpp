#include "BVH.hpp"
#include "Utilities.hpp"
#include <algorithm>

BVHNode::BVHNode(std::vector<std::shared_ptr<Hittable>>& hit_objects, size_t start, size_t end) {
  // Instead of splitting using a random axis, we will use the largest axis for the split.
  bbox = AABB::empty;
  for(size_t object_index = start; object_index < end; ++object_index) {
    bbox = AABB(bbox, hit_objects[object_index]->bounding_box());
  }

  int axis = bbox.longest_axis();

  auto comparator = (axis == 0) ? box_x_compare
                  : (axis == 1) ? box_y_compare
                                : box_z_compare;
  size_t object_span = end - start;
  
  if (object_span == 1) {
    left = right = hit_objects[start];
  } else if (object_span == 2) {
    left = hit_objects[start];
    right = hit_objects[start + 1];
  }
  else {
    std::sort(hit_objects.begin() + start, hit_objects.begin() + end, comparator);
    size_t mid = start + object_span / 2;
    left = std::make_shared<BVHNode>(hit_objects, start, mid);
    right = std::make_shared<BVHNode>(hit_objects, mid, end);
  }
}

bool BVHNode::hit(const Ray& r, Interval ray_t, HitRecord& rec) const {
  if (!bbox.hit(r, ray_t))
    return false;

  bool hit_left = left->hit(r, ray_t, rec);
  bool hit_right = right->hit(r, Interval(ray_t.min, hit_left ? rec.t : ray_t.max), rec);

  return hit_left || hit_right;
}

bool BVHNode::box_compare(
  const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis_index
) {
  const Interval a_axis_interval = a->bounding_box().axis_interval(axis_index);
  const Interval b_axis_interval = b->bounding_box().axis_interval(axis_index);
  return a_axis_interval.min < b_axis_interval.min;
}

bool BVHNode::box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
  return box_compare(a, b, 0);
}

bool BVHNode::box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
  return box_compare(a, b, 1);
}

bool BVHNode::box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b) {
  return box_compare(a, b, 2);
}