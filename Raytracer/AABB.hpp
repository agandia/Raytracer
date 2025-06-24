#pragma once

#include "Interval.hpp"
#include "Ray.hpp"
#include <glm/glm.hpp>

class AABB {
public:
  Interval x, y, z;

  AABB() = default; // The default constructor initializes an empty AABB
  AABB(const Interval& x, const Interval& y, const Interval& z);
  AABB(const glm::dvec3& a, const glm::dvec3& b);
  AABB(const AABB& box0, const AABB& box1);

  const Interval& axis_interval(int n) const;
  bool hit(const Ray& r, Interval ray_t) const;

  int longest_axis() const;

  static const AABB empty, universe;

  private:
    void pad_to_minimum();

};