#pragma once

#include "Hittable.hpp"
#include "Interval.hpp"
#include "Ray.hpp"
#include "AABB.hpp"

#include <vector>
#include <memory>

class HitPool: public Hittable {
  public:
    std::vector<std::shared_ptr<Hittable>> hit_objects;

    HitPool() = default;
    HitPool(std::shared_ptr<Hittable> object);

    void clear();
    void add(std::shared_ptr<Hittable> object);

    bool hit(const Ray& ray, Interval t, HitRecord& rec) const override;

    AABB bounding_box() const override { return bbox; }

    double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const override;

    glm::dvec3 random(const glm::dvec3& origin) const override;

  private:
    AABB bbox;
};