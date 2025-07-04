#pragma once

#include <glm/glm.hpp>

#include "OrthonormalBasis.hpp"

// Probability Density Function (PDF)
class PDF {
public:
  virtual ~PDF() {};
  virtual double value(const glm::dvec3& direction) const = 0;
  virtual glm::dvec3 generate() const = 0;
    
};

class SpherePDF : public PDF {
public:
  SpherePDF() {}

  double value(const glm::dvec3& direction) const override {
    return 1 / (4 * pi);
  }

  glm::dvec3 generate() const override {
    return random_unit_vector();
  }
};

class CosinePDF : public PDF {
public:
  CosinePDF(const glm::dvec3& w) : uvw(w) {}

  double value(const glm::dvec3& direction) const override {
    auto cosine_theta = glm::dot(glm::normalize(direction), uvw.w());
    return glm::max(0.0, cosine_theta / pi);
  }

  glm::dvec3 generate() const override {
    return uvw.transform(random_cosine_direction());
  }

private:
  OrthoNormalBasis uvw;
};

class HittablePDF : public PDF {
public:
  HittablePDF(const Hittable& objects, const glm::dvec3& origin)
    : objects(objects), origin(origin)
  {
  }

  double value(const glm::dvec3& direction) const override {
    return objects.pdf_value(origin, direction);
  }

  glm::dvec3 generate() const override {
    return objects.random(origin);
  }

private:
  const Hittable& objects;
  glm::dvec3 origin;
};

class MixturePDF: public PDF {
public:
  MixturePDF(std::shared_ptr<PDF> p0, std::shared_ptr<PDF> p1) {
    p[0] = p0;
    p[1] = p1;
  }

  double value(const glm::dvec3& direction) const override {
    return 0.5 * p[0]->value(direction) + 0.5 * p[1]->value(direction);
  }

  glm::dvec3 generate() const override {
    if (random_double() < 0.5)
      return p[0]->generate();
    else
      return p[1]->generate();
  }

private:
  std::shared_ptr<PDF> p[2];
};

class SubsurfacePDF : public PDF {
public:
  SubsurfacePDF(const glm::dvec3& normal)
    : w(normalize(normal)) {
    build_orthonormal_basis(w);
  }

  virtual double value(const glm::dvec3& direction) const override {
    // Cosine-weighted pdf: cos(theta) / pi
    double cosine = glm::dot(normalize(direction), w);
    return (cosine <= 0) ? 0 : cosine / pi;
  }
  virtual glm::dvec3 generate() const override {
    glm::dvec3 local_dir = random_cosine_direction(); // hemisphere around Z+
    return local_dir.x * u + local_dir.y * v + local_dir.z * w;
  }

private:
  glm::dvec3 u, v, w;

  void build_orthonormal_basis(const glm::dvec3& n) {
    w = normalize(n);
    glm::dvec3 a = (std::abs(w.x) > 0.9) ? glm::dvec3(0, 1, 0) : glm::dvec3(1, 0, 0);
    v = normalize(glm::cross(w, a));
    u = glm::cross(v, w);
  }
};

