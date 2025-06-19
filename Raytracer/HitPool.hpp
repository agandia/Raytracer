#pragma once

#include "Hittable.hpp"
#include "Ray.hpp"

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
};