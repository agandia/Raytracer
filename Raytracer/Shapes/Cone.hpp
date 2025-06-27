#pragma once
#include "../Hittable.hpp"
#include "../HitPool.hpp"
#include "Ellipse.hpp"
#include "Triangle.hpp"

class Cone : public Hittable {
public:
  Cone(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m);

  virtual bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override;
  virtual AABB bounding_box() const override;
  virtual double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const override;
  virtual glm::dvec3 random(const glm::dvec3& origin) const override;

private:
  std::shared_ptr<HitPool> sides;
  AABB bbox;
};