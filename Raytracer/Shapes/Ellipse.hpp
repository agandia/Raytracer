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

  double surface_area() const {
    return pi * glm::length(u) * glm::length(v);
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

    return distance_squared / (cosine * surface_area());
  }

  virtual glm::dvec3 random(const glm::dvec3& origin) const {
    // Uniformly sample a point inside a unit disk
    glm::dvec2 disk = random_in_unit_disk(); // returns vec2(x, y) with x^2 + y^2 <= 1

    glm::dvec3 sample_point = Q + disk.x * u + disk.y * v;
    return sample_point - origin;
  }

  glm::dvec3 map_exit_point(const glm::dvec3& p_entry, const glm::dvec3& normal, const glm::dvec2& disk_sample, const double radius) const override {
    // Step 1: Sample disk, remap to ellipse plane
    glm::dvec3 offset = disk_sample.x * glm::normalize(u) * glm::length(u)
      + disk_sample.y * glm::normalize(v) * glm::length(v);
    glm::dvec3 approx = p_entry + offset;

    // Step 2: Clamp to ellipse using implicit equation x^2/a^2 + y^2/b^2 <= 1
    glm::dvec3 local = approx - Q;
    double a = glm::dot(local, u) / glm::length2(u);
    double b = glm::dot(local, v) / glm::length2(v);
    double len_sq = a * a + b * b;

    if (len_sq > 1.0) {
      double scale = 1.0 / std::sqrt(len_sq);
      a *= scale;
      b *= scale;
    }

    return Q + a * u + b * v;
  }

};