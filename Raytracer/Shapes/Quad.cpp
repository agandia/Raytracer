#include "Quad.hpp"
#include "../Constants.hpp"

Quad::Quad(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, std::shared_ptr<Material> material)
  : Q(Q), u(u), v(v), material(material) 
{
  glm::dvec3 n = glm::cross(u, v);
  normal = glm::normalize(n);
  D = glm::dot(normal, Q);
  w = n / dot(n, n);
  area = glm::length(n);
  set_bounding_box();
}

void Quad::set_bounding_box()
{
  // Calculate the bounding box based on the vertices
  AABB bbox_diagonal1 = AABB(Q, Q + u + v);
  AABB bbox_diagonal2 = AABB(Q + u, Q + v);
  bbox = AABB(bbox_diagonal1, bbox_diagonal2);
}

bool Quad::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const
{
  double denom = glm::dot(glm::normalize(normal), ray.direction());

  // Early exit if ray is parallel to the plane
  if (glm::abs(denom) < 1e-8) return false;

  //Early exit if the hit point is outside of the ray interval
  double t = (D - glm::dot(glm::normalize(normal), ray.origin())) / denom;
  if (!ray_t.contains(t)) return false;

  // Determine if the hit point lies within the planar shape using its plane coordinates.
  glm::dvec3 intersection = ray.at(t);
  glm::dvec3 planar_hitpoint_vector = intersection - Q;
  double alpha = glm::dot(w, glm::cross(planar_hitpoint_vector, v));
  double beta = glm::dot(w, glm::cross(u, planar_hitpoint_vector));

  if (!is_interior(alpha, beta, rec))
    return false;

  // Ray hits the 2D shape; set the rest of the hit record and return true.
  rec.t = t;
  rec.p = intersection;
  rec.material = material;
  rec.set_face_normal(ray, glm::normalize(normal));

  if (glm::dot(rec.normal, ray.direction()) > 0)
    std::cerr << "WARNING: Backface normal detected\n";


  return true;
}

bool Quad::is_interior(double a, double b, HitRecord& rec) const
{
  Interval unit_interval = Interval(0.0, 1.0);
  // Given the git point in plane coordinates, return false if it is outside the
  // primitive, otherwise set the git record UV coordinates and return true.

  if(!unit_interval.contains(a) || !unit_interval.contains(b)) {
    return false;
  }

  rec.u = a;
  rec.v = b;
  return true;
}

double Quad::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  HitRecord rec;
  if (!this->hit(Ray(origin, direction), Interval(0.001, infinity), rec))
    return 0;

  double distance_squared = rec.t * rec.t * glm::length2(direction);
  double cosine = glm::abs(glm::dot(direction, rec.normal) / glm::length(direction));

  const double res = distance_squared / (cosine * area);
  return res;
}

glm::dvec3 Quad::random(const glm::dvec3& origin) const {
  glm::dvec3 random_point = Q + (u * random_double()) + (v * random_double());
  return random_point - origin;
}