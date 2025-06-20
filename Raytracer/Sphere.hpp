#pragma once

#include <glm/glm.hpp>
#include "Hittable.hpp"
#include "Interval.hpp"
#include "Ray.hpp"
#include <memory>

class Material;

class Sphere : public Hittable {
  public:
    Sphere() = default;
    Sphere(const glm::dvec3& center, double radius, std::shared_ptr<Material> mat) : center(center), radius(radius), mat(mat) {}

    bool hit(const Ray& ray, Interval t, HitRecord& rec) const override;

  private:
    glm::dvec3 center;
    double radius;
    std::shared_ptr<Material> mat; // Pointer to the material of the sphere
};