#pragma once

#include <glm/glm.hpp>
#include "Hittable.hpp"
#include "Ray.hpp"

class Sphere : public Hittable {
  public:
    Sphere() = default;
    Sphere(const glm::dvec3& center, double radius) : center(center), radius(radius) {}

    bool hit(const Ray& ray, double t_min, double t_max, HitRecord& rec) const override;

  private:
    glm::dvec3 center;
    double radius;
};