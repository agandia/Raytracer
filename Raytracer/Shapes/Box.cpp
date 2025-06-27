#include "Box.hpp"

Box::Box(const glm::dvec3& corner_a, const glm::dvec3& corner_b, std::shared_ptr<Material> m) {
  // returns the 3D box that contains the two corners.
  sides = std::make_shared<HitPool>();

  //Construct the two opposite vertices with min and max coordinates
  glm::dvec3 min_corner = glm::dvec3(glm::min(corner_a.x, corner_b.x), glm::min(corner_a.y, corner_b.y), glm::min(corner_a.z, corner_b.z));
  glm::dvec3 max_corner = glm::dvec3(glm::max(corner_a.x, corner_b.x), glm::max(corner_a.y, corner_b.y), glm::max(corner_a.z, corner_b.z));

  glm::dvec3 dx = glm::dvec3(max_corner.x - min_corner.x, 0.0, 0.0);
  glm::dvec3 dy = glm::dvec3(0.0, max_corner.y - min_corner.y, 0.0);
  glm::dvec3 dz = glm::dvec3(0.0, 0.0, max_corner.z - min_corner.z);

  sides->add(std::make_shared<Quad>(glm::dvec3(min_corner.x, min_corner.y, max_corner.z), dx, dy, m)); // front
  sides->add(std::make_shared<Quad>(glm::dvec3(max_corner.x, min_corner.y, max_corner.z), -dz, dy, m)); // right
  sides->add(std::make_shared<Quad>(glm::dvec3(max_corner.x, min_corner.y, min_corner.z), -dx, dy, m)); // back
  sides->add(std::make_shared<Quad>(glm::dvec3(min_corner.x, min_corner.y, min_corner.z), dz, dy, m)); // left
  sides->add(std::make_shared<Quad>(glm::dvec3(min_corner.x, max_corner.y, max_corner.z), dx, -dz, m)); // top
  sides->add(std::make_shared<Quad>(glm::dvec3(min_corner.x, min_corner.y, min_corner.z), dx, dz, m)); // bottom
}

bool Box::hit(const Ray& ray, Interval ray_t, HitRecord& rec) const {
  return sides->hit(ray, ray_t, rec);
}

double Box::pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
  return sides->pdf_value(origin, direction);
}

glm::dvec3 Box::random(const glm::dvec3& origin) const {
  return sides->random(origin);
}

AABB Box::bounding_box() const {
  return sides->bounding_box();
}