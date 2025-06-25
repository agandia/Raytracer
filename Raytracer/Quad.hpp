#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "Hittable.hpp"
#include "HitPool.hpp"
#include "Material.hpp"
#include "AABB.hpp"
#include "Ray.hpp"
#include "Interval.hpp"


class Quad : public Hittable {
  public:
    Quad(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, std::shared_ptr<Material> material);

    virtual void set_bounding_box();

    inline virtual AABB bounding_box() const override { return bbox; }

    virtual bool hit(const Ray& ray, Interval ray_t, HitRecord& rec) const override;

    virtual bool is_interior(double a, double b, HitRecord& rec) const;

  protected:
    glm::dvec3 Q; // origin of the quad
    glm::dvec3 u , v; // vectors spanning the edges of the quad
    glm::dvec3 w; // constant from a vector orthogonal to the quad w = n / dot(n, (u x v)); is this the unit/normalized normal vector of the quad?
    std::shared_ptr<Material> material; // material of the quad
    AABB bbox; // bounding box of the quad

    glm::dvec3 normal;
    double D;

};

std::shared_ptr<HitPool> box(const glm::dvec3& corner_a, const glm::dvec3& corner_b, std::shared_ptr<Material> m);

class Triangle : public Quad {
public:
  Triangle(const glm::dvec3& o, const glm::dvec3& aa, const glm::dvec3& ab, std::shared_ptr<Material> m)
    : Quad(o, aa, ab, m)
  {
  }

  virtual bool is_interior(double a, double b, HitRecord& rec) const override {
    if ((a < 0) || (b < 0) || (a + b > 1))
      return false;

    rec.u = a;
    rec.v = b;
    return true;
  }
};


class Ellipse : public Quad {
public:
  Ellipse(const glm::dvec3& center, const glm::dvec3& side_A, const glm::dvec3& side_B, std::shared_ptr<Material> m) 
    : Quad(center, side_A, side_B, m)
  {
  }

  virtual void set_bounding_box() override {
    bbox = AABB(Q - u - v, Q + u + v);
  }

  virtual bool is_interior(double a, double b, HitRecord& rec) const override {
    if ((a * a + b * b) > 1)
      return false;

    rec.u = a / 2 + 0.5;
    rec.v = b / 2 + 0.5;
    return true;
  }
};