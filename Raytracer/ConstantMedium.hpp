#pragma once

#include "Hittable.hpp"
#include "ITexture.hpp"
#include "Material.hpp"

// Sometimes called participating media or volumes. Can be used to simulate effects like fog, smoke, or other volumetric effects.
class ConstantMedium : public Hittable {
public:
  ConstantMedium(std::shared_ptr<Hittable> boundary, double density, std::shared_ptr<ITexture> tex)
    : boundary(boundary), neg_inv_density(-1 / density),
    phase_function(std::make_shared<Isotropic>(tex))
  {
  }

  ConstantMedium(std::shared_ptr<Hittable> boundary, double density, const glm::vec3& albedo)
    : boundary(boundary), neg_inv_density(-1 / density),
    phase_function(std::make_shared<Isotropic>(albedo))
  {
  }

  bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;

  inline AABB bounding_box() const override { return boundary->bounding_box(); }

private:
  std::shared_ptr<Hittable> boundary;
  double neg_inv_density;
  std::shared_ptr<Material> phase_function;
};