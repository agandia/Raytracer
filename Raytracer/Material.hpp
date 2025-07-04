#pragma once

#include "Hittable.hpp"
#include "TextureWrapper.hpp"
#include "BSSRDFSampler.hpp"
#include "Ray.hpp"
#include "Utilities.hpp"
#include "PDF.hpp"

#include <glm/glm.hpp>
#include <memory>

class ScatterRecord {
public:
  glm::vec3 attenuation;
  std::shared_ptr<PDF> pdf_ptr;
  bool skip_pdf;
  Ray skip_pdf_ray;
};

class Material {
public:
    virtual ~Material() = default;
    // Returns the attenuation and scattered ray if the ray is scattered, otherwise returns false
    virtual bool scatter(const Ray& in, const HitRecord& hit_rec, ScatterRecord& scatter_rec) const { return false; }
    virtual glm::vec3 emitted(const Ray& r_in, const HitRecord& rec, double u, double v, const glm::dvec3& point) const { return glm::vec3(0.f); }

    virtual double scattering_pdf(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const {
      return 0.0; // Default implementation returns 0, meaning no PDF is defined
    }
    
};

class Lambertian : public Material {
public:
  Lambertian(const glm::vec3& albedo) : texture(std::make_shared<SolidColorTexture>(albedo)) {}
  Lambertian(std::shared_ptr<ITexture>texture) : texture(texture) {}

  bool scatter(const Ray& in, const HitRecord& rec, ScatterRecord& scatter_rec) const override {
    OrthoNormalBasis basis(rec.normal);
    glm::dvec3 scatter_direction = basis.transform(random_cosine_direction());

    scatter_rec.attenuation = texture->color_value(rec.u, rec.v, rec.p);
    scatter_rec.pdf_ptr = std::make_shared<CosinePDF>(rec.normal);
    scatter_rec.skip_pdf = false;
    return true;
  }

  double scattering_pdf(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const override {
    double cos_theta = glm::dot(glm::normalize(rec.normal), glm::normalize(scattered.direction()));
    return (cos_theta  < 0) ? 0 : cos_theta / pi; // PDF for Lambertian scattering
  }

private:
  std::shared_ptr<ITexture> texture;
};

class Metal : public Material {
  public:
    Metal(const glm::vec3& albedo, double fuzz) : albedo(albedo), fuzz(fuzz < 1.0 ? fuzz : 1.0) {}
    
    bool scatter(const Ray& in, const HitRecord& rec, ScatterRecord& scatter_rec) const override {
      glm::dvec3 reflected = reflect(in.direction(), rec.normal);
      reflected = glm::normalize(reflected) + (fuzz * random_unit_vector()); // Normalize the reflected direction

      scatter_rec.attenuation = albedo;
      scatter_rec.pdf_ptr = nullptr; // Metal does not use a PDF for scattering
      scatter_rec.skip_pdf = true; // Metal does not use a PDF for scattering
      scatter_rec.skip_pdf_ray = Ray(rec.p, reflected, in.time()); // Store the scattered ray

      return true;
    }
  private:
    glm::vec3 albedo;
    double fuzz; // Fuzziness factor for the metal surface
};

class Dielectric : public Material {
  public:
    Dielectric(double ref_idx) : ref_idx(ref_idx) {}

    bool scatter(const Ray& in, const HitRecord& rec, ScatterRecord& scatter_rec) const override {
      scatter_rec.attenuation = glm::vec3(1.0, 1.0, 1.0); // Light is not absorbed
      scatter_rec.pdf_ptr = nullptr; // Dielectric does not use a PDF for scattering
      scatter_rec.skip_pdf = true; // Dielectric does not use a PDF for scattering

      double etai_over_etat = rec.front_face ? (1.0 / ref_idx) : ref_idx; // Determine the index of refraction

      glm::dvec3 unit_direction = glm::normalize(in.direction());

      double cos_theta = glm::min(glm::dot(-unit_direction, rec.normal), 1.0);
      double sin_theta = glm::sqrt(1.0 - cos_theta * cos_theta);

      bool cannot_refract = etai_over_etat * sin_theta > 1.0;
      glm::dvec3 direction;

      if (cannot_refract || reflectance(cos_theta, etai_over_etat) > random_double()) {
          direction = reflect(unit_direction, rec.normal);
      } else {
          direction = refract(unit_direction, rec.normal, etai_over_etat);
      }
      scatter_rec.skip_pdf_ray = Ray(rec.p, direction, in.time());
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

class DiffuseLight : public Material {
  public:
    DiffuseLight(std::shared_ptr<ITexture> texture) : texture(texture) {}
    DiffuseLight(const glm::vec3& color) : texture(std::make_shared<SolidColorTexture>(color)) {}
    glm::vec3 emitted(const Ray& r_in, const HitRecord& rec, double u, double v, const glm::dvec3& point) const override{
      if (!rec.front_face) return glm::vec3(0.f); // No emission if not front-facing
      return texture->color_value(u, v, point);
    }

  private:
    std::shared_ptr<ITexture> texture; // Texture for the emitted light color
};

class Isotropic : public Material {
public:
  Isotropic(const glm::vec3& albedo) : tex(std::make_shared<SolidColorTexture>(albedo)) {}
  Isotropic(std::shared_ptr<ITexture> tex) : tex(tex) {}

  bool scatter(const Ray& r_in, const HitRecord& rec, ScatterRecord& scatter_rec)
    const override {
    scatter_rec.attenuation = tex->color_value(rec.u, rec.v, rec.p);
    scatter_rec.pdf_ptr = std::make_shared<SpherePDF>();
    scatter_rec.skip_pdf = false;
    return true;
  }

  double scattering_pdf(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const override {
    return 1.0 / (4 * pi); // Uniform isotropic scattering PDF
  }

private:
  std::shared_ptr<ITexture> tex;
};

class SubsurfaceMaterial : public Material {
public:
  std::shared_ptr<ITexture> albedo;
  double index_of_refraction;
  double scattering_coefficient;  // σ_s
  double absorption_coefficient;  // σ_a
  double g;  // asymmetry parameter for phase function (e.g. Henyey-Greenstein)
  std::shared_ptr<BSSRDFSampler> bssrdf;

  SubsurfaceMaterial(const glm::vec3& albedo, double ior, double sigma_s, double sigma_a, double g_, std::shared_ptr<BSSRDFSampler> bssrdf_sampler)
    : albedo(std::make_shared<SolidColorTexture>(albedo)), index_of_refraction(ior), scattering_coefficient(sigma_s), absorption_coefficient(sigma_a), g(g_), bssrdf(bssrdf_sampler)
    {}

  bool scatter(const Ray& r_in, const HitRecord& rec, ScatterRecord& srec) const
  {
    // Step 1: Sample exit radius and generate disk sample
    double theta = 2 * pi * random_double();
    double r = bssrdf->sample_radius(random_double());
    glm::dvec2 disk = r * glm::dvec2(std::cos(theta), std::sin(theta));


    // Step 2: Map the exit point on the surface
    glm::dvec3 p_exit = rec.shape_ptr->map_exit_point(rec.p, rec.normal, disk, r);
    glm::dvec3 normal_exit = rec.shape_ptr->normal_at(p_exit);

    // Step 3: Compute attenuation based on distance
    double attenuation_strength = std::max(bssrdf->diffuse_profile(glm::length(p_exit - rec.p)), 1e-4);

    glm::dvec3 attenuation = albedo->color_value(rec.u, rec.v, rec.p) * (double)attenuation_strength * 1000.0;
    if (glm::length(attenuation) < 1e-6) return false; // discard very low contribution

    // Step 4: Generate cosine-weighted scattered direction from exit point
    auto pdf = std::make_shared<SubsurfacePDF>(normal_exit);
    glm::dvec3 out_dir = pdf->generate();

    srec.skip_pdf = true;
    srec.attenuation = attenuation;
    srec.pdf_ptr = pdf;
    srec.skip_pdf_ray = Ray(p_exit, out_dir, r_in.time());

    return true;
  }

  double scattering_pdf(const Ray& r_in, const HitRecord& rec, const Ray& scattered) const override {
    // Build ONB for surface normal
    OrthoNormalBasis onb(rec.normal);

    // Vector from entry point to scattered origin
    glm::dvec3 exit_offset = scattered.origin() - rec.p;

    // Project exit_offset into tangent plane (u,v)
    double x = glm::dot(exit_offset, onb.u());
    double y = glm::dot(exit_offset, onb.v());

    double r = std::sqrt(x * x + y * y);

    // Calculate radius PDF
    double radius_pdf = bssrdf->pdf_value(r);

    // Cosine PDF for outgoing direction (assuming cosine-weighted)
    double cosine = glm::dot(glm::normalize(scattered.direction()), rec.normal);
    if (cosine <= 0) return 0.0;

    double cosine_pdf = cosine / pi;

    // Total PDF is product of radius sampling and directional sampling
    return radius_pdf * cosine_pdf;
  }


  glm::vec3 emitted(const Ray& r_in, const HitRecord& rec, double u, double v, const glm::dvec3& p) const override {
    return glm::vec3(0);
  }
};
