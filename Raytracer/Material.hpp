#pragma once

#include "Hittable.hpp"
#include "TextureWrapper.hpp"
#include "Ray.hpp"
#include "Utilities.hpp"
#include <glm/glm.hpp>
#include <memory>

class Material {
public:
    virtual ~Material() = default;
    // Returns the attenuation and scattered ray if the ray is scattered, otherwise returns false
    virtual bool scatter(const Ray& in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const { return false; }
    
};

class Lambertian : public Material {
public:
  Lambertian(const glm::vec3& albedo) : texture(std::make_shared<SolidColorTexture>(albedo)) {}
  Lambertian(std::shared_ptr<ITexture>texture) : texture(texture) {}

  bool scatter(const Ray& in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override {
    glm::dvec3 scatter_direction = rec.normal + random_unit_vector();

    // Catch degenerate scatter direction
    if (near_zero(scatter_direction)) {
      scatter_direction = rec.normal; // Use the normal as a fallback direction
    }
    scattered = Ray(rec.p, scatter_direction, in.time());
    attenuation = texture->color_value(rec.u, rec.v, rec.p);
    return true;
  }

private:
  std::shared_ptr<ITexture> texture;
};

class Metal : public Material {
  public:
    Metal(const glm::vec3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}
    
    bool scatter(const Ray& in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override {
        glm::dvec3 reflected = reflect(in.direction(), rec.normal);
        reflected = glm::normalize(reflected) + (fuzz * random_unit_vector()); // Normalize the reflected direction
        scattered = Ray(rec.p, reflected, in.time());
        attenuation = albedo;
        return (dot(scattered.direction(), rec.normal) > 0);
    }
  private:
    glm::vec3 albedo;
    double fuzz; // Fuzziness factor for the metal surface
};

class Dielectric : public Material {
  public:
    Dielectric(double ref_idx) : ref_idx(ref_idx) {}

    bool scatter(const Ray& in, const HitRecord& rec, glm::vec3& attenuation, Ray& scattered) const override {
        attenuation = glm::vec3(1.0, 1.0, 1.0); // Light is not absorbed
        double etai_over_etat = rec.front_face ? (1.0 / ref_idx) : ref_idx; // Determine the index of refraction

        glm::dvec3 unit_direction = glm::normalize(in.direction());
        glm::dvec3 refracted = refract(unit_direction, rec.normal, etai_over_etat);

        double cos_theta = glm::min(glm::dot(-unit_direction, rec.normal), 1.0);
        double sin_theta = glm::sqrt(1.0 - cos_theta * cos_theta);

        bool cannot_refract = etai_over_etat * sin_theta > 1.0;
        glm::dvec3 direction;

        if (cannot_refract || reflectance(cos_theta, etai_over_etat) > random_double()) {
            direction = reflect(unit_direction, rec.normal);
        } else {
            direction = refract(unit_direction, rec.normal, etai_over_etat);
        }
        scattered = Ray(rec.p, refracted, in.time());
        return true;
    }
  private:
    double ref_idx; // Index of refraction

    static double reflectance(double cosine, double ref_idx) {
        // Schlick's approximation for reflectance
        double r0 = (1 - ref_idx) / (1 + ref_idx);
        r0 = r0 * r0;
        return r0 + (1 - r0) * pow((1 - cosine), 5);
    }
};