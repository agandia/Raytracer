#include "Sphere.hpp"
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>

bool Sphere::hit(const Ray& ray, Interval interval, HitRecord& rec) const {
  glm::dvec3 current_center = center.at(ray.time());
    glm::dvec3 oc = current_center - ray.origin(); //ray from center of sphere to ray origin
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
    if (!interval.surrounds(root)) {
        root = (h + sqrt_discriminant) / a; // Second root
        if (!interval.surrounds(root)) {
            return false; // No valid intersection in the range
        }
    }
    rec.t = root;
    rec.p = ray.at(rec.t); // Point of intersection
    glm::dvec3 outward_normal = (rec.p - current_center) / radius; // Normalized normal vector
    rec.set_face_normal(ray, outward_normal); // Normal at the intersection point
    get_sphere_uv(outward_normal, rec.u, rec.v); // Texture coordinates
    rec.material = mat; // Material of the sphere
    
    return true;
}

void Sphere::get_sphere_uv(const glm::dvec3& p, double& u, double& v) {
  // p: a given point on the sphere of radius one, centered at the origin.
  // u: returned value [0,1] of angle around the Y axis from X=-1.
  // v: returned value [0,1] of angle from Y=-1 to Y=+1.
  //     <1 0 0> yields <0.50 0.50>       <-1  0  0> yields <0.00 0.50>
  //     <0 1 0> yields <0.50 1.00>       < 0 -1  0> yields <0.50 0.00>
  //     <0 0 1> yields <0.25 0.50>       < 0  0 -1> yields <0.75 0.50>

  double theta = glm::acos(-p.y);
  double phi = atan2(-p.z, p.x) + glm::pi<double>();
  u = phi / (2.0 * glm::pi<double>());
  v = theta / glm::pi<double>();
}