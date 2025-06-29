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

  inline double surface_area() const {
    double dx = x.size();
    double dy = y.size();
    double dz = z.size();
    return 2.0 * (dx * dy + dx * dz + dy * dz);
  }

  static const AABB empty, universe;

  private:
    void pad_to_minimum();

};

AABB operator+(const AABB& bbox, const glm::dvec3& offset);

AABB operator+(const glm::dvec3& offset, const AABB& bbox);