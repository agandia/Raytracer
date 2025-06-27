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

/*
std::shared_ptr<HitPool> box(const glm::dvec3& corner_a, const glm::dvec3& corner_b, std::shared_ptr<Material> m)
{
  // returns the 3D box that contains the two corners.
  std::shared_ptr<HitPool> sides = std::make_shared<HitPool>();

  //Construct the two opposite vertices with min and max coordinates
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
}*/

/*
std::shared_ptr<HitPool> pyramid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, std::shared_ptr<Material> m)
{
  // returns the 3D pyramid that contains the two corners.
  std::shared_ptr<HitPool> Sides = std::make_shared<HitPool>();

  // Base quad (same as in box)
  Sides->add(std::make_shared<Quad>(Q, u, v, m));

  // Corners of base (winding counter-clockwise)
  glm::dvec3 b0 = Q;
  glm::dvec3 b1 = Q + u;
  glm::dvec3 b2 = Q + u + v;
  glm::dvec3 b3 = Q + v;

  // Apex point above center of base
  glm::dvec3 center = Q + 0.5 * u + 0.5 * v;
  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 apex = center + height * normal;

  Sides->add(std::make_shared<Triangle>(b0, b1 - b0, apex - b0, m));
  Sides->add(std::make_shared<Triangle>(b1, b2 - b1, apex - b1, m));
  Sides->add(std::make_shared<Triangle>(b2, b3 - b2, apex - b2, m));
  Sides->add(std::make_shared<Triangle>(b3, b0 - b3, apex - b3, m));

  return Sides;
}*/

/*
std::shared_ptr<HitPool> cylindroid(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m)
{
  auto sides = std::make_shared<HitPool>();

  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 Q_top = Q + normal * height;

  // Base and top
  sides->add(std::make_shared<Ellipse>(Q, u, v, m));
  sides->add(std::make_shared<Ellipse>(Q_top, u, v, m));

  // Side walls
  int steps = segments;
  for (int i = 0; i < steps; ++i) {
    double a0 = (i + 0) * 2.0 * pi / steps;
    double a1 = (i + 1) * 2.0 * pi / steps;

    glm::dvec3 p0 = Q + cos(a0) * u + sin(a0) * v;
    glm::dvec3 p1 = Q + cos(a1) * u + sin(a1) * v;

    glm::dvec3 p0_top = p0 + normal * height;
    glm::dvec3 p1_top = p1 + normal * height;

    sides->add(std::make_shared<Quad>(p0, p1 - p0, p0_top - p0, m));
  }

  return sides;
}
*/

/*
std::shared_ptr<HitPool> cone(const glm::dvec3& Q, const glm::dvec3& u, const glm::dvec3& v, double height, int segments, std::shared_ptr<Material> m)
{
  auto faces = std::make_shared<HitPool>();

  // Ellipse normal and apex
  glm::dvec3 normal = glm::normalize(glm::cross(u, v));
  glm::dvec3 apex = Q + normal * height;

  // Base ellipse
  faces->add(std::make_shared<Ellipse>(Q, u, v, m));

  // Side triangles
  int steps = segments;
  for (int i = 0; i < steps; ++i) {
    double a0 = (i + 0) * 2.0 * pi / steps;
    double a1 = (i + 1) * 2.0 * pi / steps;

    glm::dvec3 p0 = Q + cos(a0) * u + sin(a0) * v;
    glm::dvec3 p1 = Q + cos(a1) * u + sin(a1) * v;

    faces->add(std::make_shared<Triangle>(p0, p1 - p0, apex - p0, m));
  }

  return faces;
}
*/