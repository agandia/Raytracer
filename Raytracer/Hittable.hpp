#pragma once

//#include <glm/glm.hpp>
#include "Ray.hpp"

class HitRecord {
  public:
    glm::dvec3 p;       ///< Point of intersection
    glm::dvec3 normal;  ///< Normal at the intersection point
    double t;           ///< Ray parameter at the intersection
    bool front_face;    ///< Indicates if the ray hit the front face of the object

    void set_face_normal(const Ray& r, const glm::dvec3& outward_normal) {
        front_face = glm::dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }

};

class Hittable {
  public:
    // Check if the ray intersects with the object
    virtual bool hit(const Ray& r, double t_min, double t_max, HitRecord& rec) const = 0;
    // Virtual destructor for proper cleanup of derived classes
    virtual ~Hittable() = default;
};