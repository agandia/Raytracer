#pragma once

#include <vector>
#include <memory>

#include "AABB.hpp"
#include "Ray.hpp"
#include "Interval.hpp"
#include "Hittable.hpp"
#include "HitPool.hpp"

class BVHNode : public Hittable {
  public:
    BVHNode() = default;
    BVHNode(HitPool list) : BVHNode(list.hit_objects, 0, list.hit_objects.size()) {
      // There's a C++ subtlety here. This constructor (without span indices) creates an
      // implicit copy of the hittable list, which we will modify. The lifetime of the copied
      // list only extends until this constructor exits. That's OK, because we only need to
      // persist the resulting bounding volume hierarchy.
    }
    BVHNode(std::vector<std::shared_ptr<Hittable>>& hit_objects, size_t start, size_t end);

    bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;

    AABB bounding_box() const override { return bbox; };

  private:
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB bbox;

    static bool box_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b, int axis_index);

    static bool box_x_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);

    static bool box_y_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);

    static bool box_z_compare(const std::shared_ptr<Hittable> a, const std::shared_ptr<Hittable> b);
};
