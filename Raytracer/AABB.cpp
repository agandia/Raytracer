#include "AABB.hpp"

AABB::AABB(const Interval& x, const Interval& y, const Interval& z) :
  x(x), y(y), z(z) {
  pad_to_minimum();
}

AABB::AABB(const glm::dvec3& a, const glm::dvec3& b) {
  // Treat the two points a and b as extrema for the bounding box, so we don't require a
  // particular minimum/maximum coordinate order.

  x = (a[0] <= b[0]) ? Interval(a[0], b[0]) : Interval(b[0], a[0]);
  y = (a[1] <= b[1]) ? Interval(a[1], b[1]) : Interval(b[1], a[1]);
  z = (a[2] <= b[2]) ? Interval(a[2], b[2]) : Interval(b[2], a[2]);

  pad_to_minimum();
}

AABB::AABB(const AABB& box0, const AABB& box1) {
  x = Interval(box0.x, box1.x);
  y = Interval(box0.y, box1.y);
  z = Interval(box0.z, box1.z);
}

const Interval& AABB::axis_interval(int n) const {
  if (n == 1) return y;
  if (n == 2) return z;
  return x;
}

bool AABB::hit(const Ray& ray, Interval ray_t) const {
  const glm::dvec3& ray_origin = ray.origin();
  const glm::dvec3& ray_direction = ray.direction();

  for(int axis = 0; axis < 3; ++axis) {
    const Interval& ax = axis_interval(axis);
    const double adinv = 1.0 / ray_direction[axis];

    double t0 = (ax.min - ray_origin[axis]) * adinv;
    double t1 = (ax.max - ray_origin[axis]) * adinv;

    if(t0 < t1) {
      if (t0 > ray_t.min) ray_t.min = t0;
      if (t1 < ray_t.max) ray_t.max = t1;
    } else {
      if (t1 > ray_t.min) ray_t.min = t1;
      if (t0 < ray_t.max) ray_t.max = t0;
    }

    if(ray_t.max <= ray_t.min) {
      return false; // No intersection
    }
  }
  return true;
}

int AABB::longest_axis() const
{
  // Returns the index of the longest axis of the bounding box.

  if (x.size() > y.size())
    return x.size() > z.size() ? 0 : 2;
  else
    return y.size() > z.size() ? 1 : 2;
}

void AABB::pad_to_minimum()
{
  // Ensure that the AABB has a minimum size in each dimension.
  // If any dimension is empty, we expand it to a minimum size.
  double delta = 0.0001; // Minimum size for each dimension

  if (x.size() < delta) x = x.expand(delta);
  if (y.size() < delta) y = y.expand(delta);
  if (z.size() < delta) z = z.expand(delta);
}


AABB operator+(const AABB& bbox, const glm::dvec3& offset) {
  return AABB(bbox.x + offset.x, bbox.y + offset.y, bbox.z + offset.z);
}

AABB operator+(const glm::dvec3& offset, const AABB& bbox) {
  return bbox + offset;
}

const AABB AABB::empty = AABB::AABB(Interval::empty, Interval::empty, Interval::empty);
const AABB AABB::universe = AABB::AABB(Interval::universe, Interval::universe, Interval::universe);