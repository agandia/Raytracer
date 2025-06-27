#pragma once 
#include "Quad.hpp"

class Triangle : public Quad {
public:
  Triangle(const glm::dvec3& o, const glm::dvec3& aa, const glm::dvec3& ab, std::shared_ptr<Material> m)
    : Quad(o, aa, ab, m)
  {
  }

  virtual bool is_interior(double a, double b, HitRecord& rec) const override {
    if ((a < 0) || (b < 0) || (a + b > 1))
      return false;

    rec.u = a;
    rec.v = b;
    return true;
  }
};
