#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "Hittable.hpp"
#include "Material.hpp"
#include "AABB.hpp"
#include "Ray.hpp"
#include "Interval.hpp"


class Quad : public Hittable {
  public:
    Quad(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, std::shared_ptr<Material> material);

    virtual void set_bounding_box();

    inline AABB bounding_box() const override { return bbox; }

    bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override;

    virtual bool is_interior(double a, double b, HitRecord& rec) const;

  private:
    glm::dvec3 Q; // origin of the quad
    glm::dvec3 u , v; // vectors spanning the edges of the quad
    glm::dvec3 w; // constant from a vector orthogonal to the quad w = n / dot(n, (u x v)); is this the unit/normalized normal vector of the quad?
    std::shared_ptr<Material> material; // material of the quad
    AABB bbox; // bounding box of the quad

    glm::dvec3 normal;
    double D;

};