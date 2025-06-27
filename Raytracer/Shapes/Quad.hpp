#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "../Hittable.hpp"
#include "../HitPool.hpp"
#include "../Material.hpp"
#include "../AABB.hpp"
#include "../Ray.hpp"
#include "../Interval.hpp"


class Quad : public Hittable {
  public:
    Quad(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, std::shared_ptr<Material> material);

    virtual void set_bounding_box();

    inline virtual AABB bounding_box() const override { return bbox; }

    virtual bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override;

    virtual bool is_interior(double a, double b, HitRecord& rec) const;

    virtual double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const override;

    virtual glm::dvec3 random(const glm::dvec3& origin) const override;

  protected:
    glm::dvec3 Q; // origin of the quad
    glm::dvec3 u , v; // vectors spanning the edges of the quad
    glm::dvec3 w; // constant from a vector orthogonal to the quad w = n / dot(n, (u x v)); is this the unit/normalized normal vector of the quad?
    std::shared_ptr<Material> material; // material of the quad
    AABB bbox; // bounding box of the quad

    glm::dvec3 normal;
    double area;
    double D;

};
/*
std::shared_ptr<HitPool> box(const glm::dvec3& corner_a, const glm::dvec3& corner_b, std::shared_ptr<Material> m);

std::shared_ptr<HitPool> pyramid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double apex, std::shared_ptr<Material> m);

std::shared_ptr<HitPool> cylindroid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height,int segments, std::shared_ptr<Material> m);

std::shared_ptr<HitPool> cone(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m);
*/