#include "Quad.hpp"

Quad::Quad(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, std::shared_ptr<Material> material)
  : Q(Q), u(u), v(v), material(material) 
{
  glm::dvec3 n = glm::cross(u, v);
  normal = glm::normalize(n);
  D = glm::dot(normal, Q);
  w = n / dot(n, n);
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
  double denom = glm::dot(normal, ray.direction());

  // Early exit if ray is parallel to the plane
  if (glm::abs(denom) < 1e-8) return false;

  //Early exit if the hit point is outside of the ray interval
  double t = (D - glm::dot(normal, ray.origin())) / denom;
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
  rec.set_face_normal(ray, normal);

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

std::shared_ptr<HitPool> box(const glm::dvec3& corner_a, const glm::dvec3& corner_b, std::shared_ptr<Material> m)
{
  // returns the 3D box that contains the two corners.
  std::shared_ptr<HitPool> sides = std::make_shared<HitPool>();

  //Construct the two oposite vertices with min and max coordinates
  glm::dvec3 min_corner = glm::dvec3(glm::min(corner_a.x, corner_b.x), glm::min(corner_a.y, corner_b.y), glm::min(corner_a.z, corner_b.z));
  glm::dvec3 max_corner = glm::dvec3(glm::max(corner_a.x, corner_b.x), glm::max(corner_a.y, corner_b.y), glm::max(corner_a.z, corner_b.z));

  glm::dvec3 dx = glm::dvec3(max_corner.x - min_corner.x, 0.0, 0.0);
  glm::dvec3 dy = glm::dvec3(0.0, max_corner.y - min_corner.y, 0.0);
  glm::dvec3 dz = glm::dvec3(0.0, 0.0, max_corner.z - min_corner.z);

  sides->add(std::make_shared<Quad>(glm::dvec3(min_corner.x, min_corner.y, max_corner.z),  dx,  dy, m)); // front
  sides->add(std::make_shared<Quad>(glm::dvec3(max_corner.x, min_corner.y, max_corner.z), -dz,  dy, m)); // right
  sides->add(std::make_shared<Quad>(glm::dvec3(max_corner.x, min_corner.y, min_corner.z), -dx,  dy, m)); // back
  sides->add(std::make_shared<Quad>(glm::dvec3(min_corner.x, min_corner.y, min_corner.z),  dz,  dy, m)); // left
  sides->add(std::make_shared<Quad>(glm::dvec3(min_corner.x, max_corner.y, max_corner.z),  dx, -dz, m)); // top
  sides->add(std::make_shared<Quad>(glm::dvec3(min_corner.x, min_corner.y, min_corner.z),  dx,  dz, m)); // bottom

  return sides;
}
