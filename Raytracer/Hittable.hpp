#pragma once

#include "Ray.hpp"
#include "Interval.hpp"
#include "AABB.hpp"
#include <memory>
#include <glm/glm.hpp>

class Material; // Forward declaration of Material class

class HitRecord {
  public:
    glm::dvec3 p;                       ///< Point of intersection
    glm::dvec3 normal;                  ///< Normal at the intersection point
    std::shared_ptr<Material> material; ///< Material at the intersection point
    double t;                           ///< Time t at the intersection
    double u;                           ///< Texture coordinate u
    double v;                           ///< Texture coordinate v
    bool front_face;                    ///< Indicates if the ray hit the front face of the object

    void set_face_normal(const Ray& r, const glm::dvec3& outward_normal) {
        front_face = glm::dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }

};

class Hittable {
  public:
    // Check if the ray intersects with the object
    virtual bool hit(const Ray& r, Interval t, HitRecord& rec) const = 0;
    // Virtual destructor for proper cleanup of derived classes
    virtual ~Hittable() = default;

    virtual AABB bounding_box() const = 0; ///< Get the bounding box of the object

    virtual double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const {
      return 0.0;
    }

    virtual glm::dvec3  random(const glm::dvec3& origin) const {
      return glm::dvec3(1, 0, 0);
    }
};

class Translate : public Hittable {
  public:
    Translate(std::shared_ptr<Hittable> hittable, const glm::dvec3& offset)
        : hittable(hittable), offset(offset) {
      bbox = hittable->bounding_box() + offset;
    }
  
    inline AABB bounding_box() const override { return bbox; }
  
    bool hit(const Ray& r, Interval t, HitRecord& rec) const override;

  private:
    std::shared_ptr<Hittable> hittable; ///< Pointer to the hittable object
    glm::dvec3 offset;              ///< Offset for translation
    AABB bbox;                     ///< Bounding box of the translated object
};

// A bit limiting in terms of rotations but most objects we want to render are presented in the xz plane.
class RotateYAxis : public Hittable {
public:
  RotateYAxis(std::shared_ptr<Hittable> object, double angle);
  bool hit(const Ray& r, Interval ray_t, HitRecord& rec) const override;
  inline AABB bounding_box() const override { return bbox; }
  double pdf_value(const glm::dvec3& origin, const glm::dvec3& direction) const override;
  glm::dvec3 random(const glm::dvec3& origin) const override;
private:
  std::shared_ptr<Hittable> hittable;
  double sin_theta;
  double cos_theta;
  AABB bbox;

  glm::dvec3 rotate_y(const glm::dvec3& p) const;
  glm::dvec3 inverse_rotate_y(const glm::dvec3& p) const;
};