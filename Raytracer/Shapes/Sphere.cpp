#include "Sphere.hpp"
#include "../Utilities.hpp"
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
    rec.shape_ptr = this;

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

glm::dvec3 Sphere::random_to_sphere(double radius, double distance_squared)
{
  double r1 = random_double();
  double r2 = random_double();
  double z = 1 + r2 * (std::sqrt(1 - radius * radius / distance_squared) - 1);

  auto phi = 2 * pi * r1;
  auto x = std::cos(phi) * std::sqrt(1 - z * z);
  auto y = std::sin(phi) * std::sqrt(1 - z * z);

  return glm::dvec3(x, y, z);
}

double Sphere::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  // TODO: This method only works for stationary spheres.

  HitRecord rec;
  if (!this->hit(Ray(origin, direction), Interval(0.001, infinity), rec))
    return 0;

  double dist_squared = glm::length2(center.at(0) - origin);
  double cos_theta_max = std::sqrt(1 - radius * radius / dist_squared);
  double solid_angle = 2 * pi * (1 - cos_theta_max);

  return  1 / solid_angle;
}

glm::dvec3 Sphere::random(const glm::dvec3& origin) const {
  glm::dvec3 direction = center.at(0.0) - origin;
  double distance_squared = glm::length2(direction);
  OrthoNormalBasis basis(direction);
  return basis.transform(random_to_sphere(radius, distance_squared));
}

glm::dvec3 Sphere::map_exit_point(const glm::dvec3& p_entry, const glm::dvec3& normal, const glm::dvec2& disk_sample, const double sample_radius) const {
  OrthoNormalBasis onb(normal);
  // Offset vector in tangent plane scaled by sample_radius (the radius from diffusion profile)
  glm::dvec3 offset = sample_radius * (disk_sample.x * onb.u() + disk_sample.y * onb.v());

  // Approximate local point on tangent plane
  glm::dvec3 tangent_point = p_entry + offset;

  // Direction from center to tangent_point
  glm::dvec3 dir = glm::normalize(tangent_point - center.at(0));

  // Exit point on sphere surface at radius along this direction
  glm::dvec3 p_exit = center.at(0) + 1.004 *(radius * dir);

  // Slightly offset p_exit along normal to avoid self-intersection
  //p_exit += normal * 1e-4;

  return p_exit;
}


glm::dvec3 Sphere::normal_at(const glm::dvec3& p) const {
  return glm::normalize(p - center.at(0));
}
