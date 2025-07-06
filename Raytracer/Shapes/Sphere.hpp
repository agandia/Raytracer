#pragma once

#include <glm/glm.hpp>
#include <memory>

#include "../Hittable.hpp"
#include "../AABB.hpp"
#include "../Interval.hpp"
#include "../Ray.hpp"
#include "../OrthoNormalBasis.hpp"

class Material;

class Sphere : public Hittable {
  public:
    Sphere() = default;
    // Stationary sphere
    Sphere(const glm::dvec3& center, double radius, std::shared_ptr<Material> mat) : center(center, glm::dvec3(0.0)), radius(radius), mat(mat) {
      glm::dvec3 rvec = glm::dvec3(radius, radius, radius);
      bbox = AABB(center - rvec, center + rvec);
    }
    // Dynamic sphere with a moving center
    Sphere(const glm::dvec3& center_t1, glm::dvec3& center_t2, double radius, std::shared_ptr<Material> mat) : center(center_t1, center_t2 - center_t1), radius(radius), mat(mat) {
      glm::dvec3 rvec = glm::dvec3(radius, radius, radius);
      AABB box1(center.origin() - rvec, center.direction() + rvec); //center.origin() == center.at(0) but without doing an unnecessary multiplication and addition
      AABB box2(center.at(1) - rvec, center.origin() + rvec);
      bbox = AABB(box1, box2);
    }

    bool hit(const Ray& ray, Interval t, HitRecord& rec) const override;

    bool contains(const glm::dvec3& p) const override;

    AABB bounding_box() const override { return bbox; }

    double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const override;

    glm::dvec3 random(const glm::dvec3& origin) const override;

    glm::dvec3 normal_at(const glm::dvec3& p) const override;

  private:
    static void get_sphere_uv(const glm::dvec3& p, double& u, double& v);
    static glm::dvec3 random_to_sphere(double radius, double distance_squared);
    Ray center;
    double radius;
    std::shared_ptr<Material> mat; // Pointer to the material of the sphere
    AABB bbox; // Axis-aligned bounding box for the sphere
};