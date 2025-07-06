#pragma once
#include "../Hittable.hpp"
#include "../BVH.hpp"
#include "Quad.hpp"
#include "Triangle.hpp"

class Pyramid : public Hittable {
public:
  Pyramid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, std::shared_ptr<Material> m);

  virtual bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override;
  virtual bool contains(const glm::dvec3& p) const override;
  virtual AABB bounding_box() const override;
  virtual double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const override;
  virtual glm::dvec3 random(const glm::dvec3& origin) const override;

private:
  std::shared_ptr<Hittable> sides_bvh;
  std::vector<std::shared_ptr<Hittable>> face_list;
  std::vector<double> surface_areas;
  AABB bbox;

  glm::dvec3 b0_, b1_, b2_, b3_, apex_;
  glm::dvec3 base_n_;

};