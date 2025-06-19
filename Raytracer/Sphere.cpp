#include "Sphere.hpp"

#include <glm/gtx/norm.hpp>

bool Sphere::hit(const Ray& ray, double t_min, double t_max, HitRecord& rec) const {
    glm::dvec3 oc = center - ray.origin(); //ray from center of sphere to ray origin
    double a = glm::length2(ray.direction());
    double h = glm::dot(ray.direction(), oc);
    double c = glm::length2(oc) - radius * radius;
    double discriminant = h * h - a * c;

    if (discriminant < 0) {
      return false; // No intersection
    }

    double sqrt_discriminant = glm::sqrt(discriminant);

    // Find the nearest root that lies in the acceptable range
    double root = (h - sqrt_discriminant) / a; // First root
    if (root <= t_min || t_max <= root) {
        root = (h + sqrt_discriminant) / a; // Second root
        if (root <= t_min || t_max <= root) {
            return false; // No valid intersection in the range
        }
    }
    rec.t = root;
    rec.p = ray.at(rec.t); // Point of intersection
    glm::vec3 outward_normal = (rec.p - center) / radius; // Normalized normal vector
    rec.set_face_normal(ray, outward_normal); // Normal at the intersection point
    
    return true;
}