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

  // Triangle.cpp
  virtual double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
    Ray r(origin, direction);
    HitRecord rec;

    if (!this->hit(r, Interval(0.001, infinity), rec))
      return 0;

    double distance_squared = rec.t * rec.t * glm::length2(direction);
    double cosine = glm::abs(glm::dot(direction, rec.normal) / glm::length(direction));

    // Triangle area = 0.5 * |u × v|, already stored in area from Quad constructor
    return distance_squared / (cosine * area);
  }

  virtual glm::dvec3 random(const glm::dvec3& origin) const {
    // Uniform random point in triangle using barycentric sampling
    double sqrt_r1 = std::sqrt(random_double());
    double r2 = random_double();
    double a = 1.0 - sqrt_r1;
    double b = sqrt_r1 * (1.0 - r2);
    double c = sqrt_r1 * r2;

    glm::dvec3 random_point = Q + a * glm::dvec3(0.0) + b * u + c * v; // u and v come from Quad
    return random_point - origin;
  }

};
