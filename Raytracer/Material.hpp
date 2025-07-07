#pragma once

#include "Hittable.hpp"
#include "TextureWrapper.hpp"
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
    virtual bool scatter(const Ray& /*in*/, const HitRecord& /*hit_rec*/, ScatterRecord& /*scatter_rec*/) const { return false; }
    virtual glm::vec3 emitted(const Ray& /*r_in*/, const HitRecord& /*rec*/, double /*u*/, double /*v*/, const glm::dvec3& /*point*/) const { return glm::vec3(0.f); }

    virtual double scattering_pdf(const Ray& /*r_in*/, const HitRecord& /*rec*/, const Ray& /*scattered*/) const {
      return 0.0; // Default implementation returns 0, meaning no PDF is defined
    }
    
};

class Lambertian : public Material {
public:
  Lambertian(const glm::vec3& albedo) : texture(std::make_shared<SolidColorTexture>(albedo)) {}
  Lambertian(std::shared_ptr<ITexture>texture) : texture(texture) {}

  bool scatter(const Ray& /*in*/, const HitRecord& rec, ScatterRecord& scatter_rec) const override {
    OrthoNormalBasis basis(rec.normal);
    glm::dvec3 scatter_direction = basis.transform(random_cosine_direction());

    scatter_rec.attenuation = texture->color_value(rec.u, rec.v, rec.p);
    scatter_rec.pdf_ptr = std::make_shared<CosinePDF>(rec.normal);
    scatter_rec.skip_pdf = false;
    return true;
  }

  double scattering_pdf(const Ray& /*r_in*/, const HitRecord& rec, const Ray& scattered) const override {
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
    glm::vec3 emitted(const Ray& /*r_in*/, const HitRecord& rec, double u, double v, const glm::dvec3& point) const override {
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

  bool scatter(const Ray& /*r_in*/, const HitRecord& rec, ScatterRecord& scatter_rec) const override {
    scatter_rec.attenuation = tex->color_value(rec.u, rec.v, rec.p);
    scatter_rec.pdf_ptr = std::make_shared<SpherePDF>();
    scatter_rec.skip_pdf = false;
    return true;
  }

  double scattering_pdf(const Ray& /*r_in*/, const HitRecord& /*rec*/, const Ray& /*scattered*/) const override {
    return 1.0 / (4 * pi); // Uniform isotropic scattering PDF
  }

private:
  std::shared_ptr<ITexture> tex;
};

class SubsurfaceMaterial : public Material {
public:
  SubsurfaceMaterial(glm::dvec3 sigma_s, glm::dvec3 sigma_a, double g, double eta)
    : sigma_s(sigma_s), sigma_a(sigma_a), g(g), eta(eta)
  {
  }

  /* ---------- main entry ---------- */
  bool scatter(const Ray& r_in, const HitRecord& rec, ScatterRecord& srec) const override
  {
    /* 1. Fresnel split (same as before) ----------------------------- */
    glm::dvec3 wo = glm::normalize(r_in.direction());
    glm::dvec3 n = rec.normal;
    double cos_i = std::clamp(glm::dot(-wo, n), 0.0, 1.0);
    double eta_i = rec.front_face ? 1.0 : eta;
    double eta_t = rec.front_face ? eta : 1.0;
    double eta_rel = eta_i / eta_t;
    const Hittable* shape = rec.shape_ptr;

    double R0 = (eta_rel - 1) * (eta_rel - 1) / ((eta_rel + 1) * (eta_rel + 1));
    double Fr = R0 + (1 - R0) * std::pow(1 - cos_i, 5);
    Fr *= 0.08;                                   // damp shiny look

    if (rnd() < Fr) {                             // reflect
      glm::dvec3 refl = glm::reflect(wo, n);
      srec.skip_pdf_ray = Ray(rec.p, refl);
      srec.skip_pdf = true;
      srec.attenuation = glm::vec3(1);
      return true;
    }

    /* 2. Refract into the medium ----------------------------------- */
    glm::dvec3 wi = glm::refract(wo, n, eta_rel);
    if (near_zero(wi)) return false;              // TIR unlikely

    /* 3. Random‑walk inside 'shape' -------------------------------- */
    glm::dvec3 Tr(1);
    glm::dvec3 pos = rec.p + 1e-4 * wi;
    glm::dvec3 dir = wi;

    for (int bounce = 0; bounce < 512; ++bounce) {
      glm::dvec3 sigma_t = sigma_s + sigma_a;
      double sum_sigma = sigma_t.x + sigma_t.y + sigma_t.z;
      double xi = rnd() * sum_sigma;
      int    ch = (xi < sigma_t.x) ? 0 : (xi < sigma_t.x + sigma_t.y ? 1 : 2);
      double sigma_t_ch = (ch == 0 ? sigma_t.x : (ch == 1 ? sigma_t.y : sigma_t.z));

      double t = -std::log(1 - rnd()) / sigma_t_ch;
      pos += dir * t;

      /* exit test for arbitrary shape */
      if (!shape->contains(pos)) {
        // push the point slightly back to the boundary normal
        Ray exit_ray(pos, glm::normalize(pos - rec.p)); // crude normal
        HitRecord exit_rec;
        shape->hit(exit_ray, Interval(1e-4, infinity), exit_rec);

        glm::dvec3 out = sample_hg(g, exit_rec.normal);
        srec.skip_pdf_ray = Ray(pos, out);
        srec.skip_pdf = true;
        srec.attenuation = glm::vec3(Tr);
        return true;
      }

      /* absorption & single‑scatter albedo */
      glm::dvec3 att = glm::exp(-sigma_a * t);
      Tr *= att * (sigma_s / sigma_t);

      /* Russian roulette */
      if (bounce > 8) {
        double q = std::max({ Tr.x,Tr.y,Tr.z });
        if (rnd() > q) return false;
        Tr /= q;
      }
      /* new direction */
      dir = sample_hg(g, dir);
    }
    return false; // path exceeded bounce budget
  }

private:
  // Henyey–Greenstein Phase Function -------------------------------------------
  glm::dvec3 sample_hg(double hg, glm::dvec3& wo) const {
    double xi = rnd();
    double cos_theta;
    if (std::abs(hg) < 1e-3) cos_theta = 1 - 2 * xi;
    else {
      double sq = (1 - hg * hg) / (1 - hg + 2 * hg * xi);
      cos_theta = (1 + hg * hg - sq * sq) / (2 * hg);
    }
    double sin_theta = std::sqrt(std::max(0.0, 1 - cos_theta * cos_theta));
    double phi = 2 * pi * rnd();

    // build local frame around wo
    glm::dvec3 w = glm::normalize(wo);
    glm::dvec3 u = glm::normalize(glm::cross((std::abs(w.x) > .1 ? glm::dvec3(0, 1, 0)
      : glm::dvec3(1, 0, 0)), w));
    glm::dvec3 v = glm::cross(w, u);

    return glm::normalize(cos_theta * w +
      sin_theta * std::cos(phi) * u +
      sin_theta * std::sin(phi) * v);
  }

  glm::dvec3 sigma_s, sigma_a;
  double g, eta;
};