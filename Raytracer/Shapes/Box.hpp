#pragma once
#include <array>
#include "../Hittable.hpp"
#include "../BVH.hpp"
#include "Quad.hpp"

class Box : public Hittable {
public:
  Box(const glm::dvec3& corner_a, const glm::dvec3& corner_b, std::shared_ptr<Material> m);

  virtual bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override;
  virtual bool contains(const glm::dvec3& p) const override;
  virtual AABB bounding_box() const override;
  virtual double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const override;
  virtual glm::dvec3 random(const glm::dvec3& origin) const override;

private:
  std::shared_ptr<Hittable> sides_bvh;
  std::vector<std::shared_ptr<Hittable>> face_list;
  std::array<double, 6> face_weights;
  AABB bbox;
};