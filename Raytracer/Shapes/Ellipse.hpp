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

  virtual double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
    Ray r(origin, direction);
    HitRecord rec;
    if (!this->hit(r, Interval(0.001, infinity), rec))
      return 0;

    double distance_squared = rec.t * rec.t * glm::length2(direction);
    double cosine = glm::abs(glm::dot(direction, rec.normal) / glm::length(direction));

    return distance_squared / (cosine * area);
  }

  virtual glm::dvec3 random(const glm::dvec3& origin) const {
    // Uniformly sample a point inside a unit disk
    glm::dvec2 disk = random_in_unit_disk(); // returns vec2(x, y) with x^2 + y^2 <= 1

    glm::dvec3 sample_point = Q + disk.x * u + disk.y * v;
    return sample_point - origin;
  }

};