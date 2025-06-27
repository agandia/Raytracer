#pragma once

#include "Quad.hpp"

class Ellipse : public Quad {
public:
  Ellipse(const glm::dvec3& center, const glm::dvec3& side_A, const glm::dvec3& side_B, std::shared_ptr<Material> m)
    : Quad(center, side_A, side_B, m)
  {
  }

  virtual void set_bounding_box() override {
    bbox = AABB(Q - u - v, Q + u + v);
  }

  virtual bool is_interior(double a, double b, HitRecord& rec) const override {
    if ((a * a + b * b) > 1)
      return false;

    rec.u = a / 2 + 0.5;
    rec.v = b / 2 + 0.5;
    return true;
  }
};