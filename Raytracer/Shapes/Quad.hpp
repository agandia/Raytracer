#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "../Hittable.hpp"
#include "../Material.hpp"
#include "../AABB.hpp"
#include "../Ray.hpp"
#include "../Interval.hpp"


class Quad : public Hittable {
  public:
    Quad(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, std::shared_ptr<Material> material);

    virtual void set_bounding_box();

    inline virtual AABB bounding_box() const override { return bbox; }

    inline double surface_area() const { return bbox.surface_area(); }

    virtual bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override;

    virtual bool is_interior(double a, double b, HitRecord& rec) const;

    virtual double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const override;

    virtual glm::dvec3 random(const glm::dvec3& origin) const override;

    virtual glm::dvec3 normal_at(const glm::dvec3& p) const override;

  protected:
    glm::dvec2 world_to_uv(const glm::dvec3& p) const;

    glm::dvec3 uv_to_world(const glm::dvec2& uv) const;

    glm::dvec3 Q; // origin of the quad
    glm::dvec3 u , v; // vectors spanning the edges of the quad
    glm::dvec3 w; // constant from a vector orthogonal to the quad w = n / dot(n, (u x v)); is this the unit/normalized normal vector of the quad?
    std::shared_ptr<Material> material; // material of the quad
    AABB bbox; // bounding box of the quad

    glm::dvec3 normal;
    double area;
    double D;

};