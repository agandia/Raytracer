#pragma once
#include "../Hittable.hpp"
#include "../BVH.hpp"
#include "Ellipse.hpp"
#include "Quad.hpp"

class Cylindroid : public Hittable {
public:
  Cylindroid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m);

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
  
  glm::dvec3 base_Q_;
  glm::dvec3 axis_w_;
  glm::dvec3 b_u_, b_v_;
  double     h_;
  double     r_u_, r_v_;

};
