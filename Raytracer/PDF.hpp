#pragma once

#include <glm/glm.hpp>

#include "OrthonormalBasis.hpp"
#include "HitPool.hpp"

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