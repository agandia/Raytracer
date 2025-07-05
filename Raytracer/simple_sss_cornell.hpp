// simple_sss_cornell.cpp
// -------------------------------------------
// A *single‑file*, header‑only‑friendly Cornell‑box path‑tracer that puts a
// wax‑/jade‑like translucent sphere in the centre.  All the heavy‑lifting
// math (glm::dvec3) lives in your own `vec3.hpp` – drop this next to it, compile,
// and you’ll get a vivid subsurface‑scattering render in a minute or two.
//
//   g++ -std=c++17 -O3 simple_sss_cornell.cpp -o render
//   ./render > image.ppm
//
// ‑‑ About the SSS implementation ‑‑
// We wrap a geometric *sphere* with a `constant_medium` that uses a
// *Henyey‑Greenstein* phase function (g ≈ 0.8 ⇒ strong forward scatter),
// low absorption density, and a green albedo – visually similar to jade.
// Because the medium is inside the sphere’s volume only, light enters via
// refraction at the surface, bounces around in the participating medium,
// and exits with that characteristic soft, waxy falloff.
// -------------------------------------------
#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/norm.hpp>
#include <chrono>
#include <algorithm>
#include <cmath>
#include <cstdint>
#include <fstream>
#include <iostream>
#include <limits>
#include <memory>
#include <random>
#include <vector>

// Aliases for brevity ---------------------------------------------------------
const double INF = std::numeric_limits<double>::infinity();
const double PI = 3.1415926535897932385;

// Utility ---------------------------------------------------------------------
thread_local std::mt19937_64 rng(2025 + omp_get_thread_num());
thread_local std::uniform_real_distribution<double> uni(0.0, 1.0);
inline double rnd() { return uni(rng); }

inline double clamp(double x, double lo, double hi) { return std::max(lo, std::min(x, hi)); }

// Ray -------------------------------------------------------------------------
struct ray {
  glm::dvec3 orig;
  glm::dvec3   dir;
  ray() : orig(), dir() {}
  ray(const glm::dvec3& o, const glm::dvec3& d) : orig(o), dir(d) {}
  glm::dvec3 at(double t) const { return orig + t * dir; }
};

// Forward decls ---------------------------------------------------------------
struct hit_record;
class material;

// Scatter‑pack returned by materials
struct scatter_pack {
  ray     spec;        // scattered ray
  glm::vec3  atten;       // multiplicative glm::vec3 term
  bool    is_specular; // true for purely specular paths (no PDF)
};

// Abstract material -----------------------------------------------------------
class material {
public:
  virtual bool scatter(const ray& r_in, const hit_record& rec, scatter_pack& s) const = 0;
  virtual glm::vec3 emitted() const { return { 0,0,0 }; }
};

// Hittable base ---------------------------------------------------------------
struct hit_record {
  glm::dvec3               p;
  glm::dvec3                 normal;
  std::shared_ptr<material> mat;
  double               t;
  bool                 front;
  void set_face(const ray& r, const glm::dvec3& out_n) {
    front = dot(r.dir, out_n) < 0;
    normal = front ? out_n : -out_n;
  }
};

class hittable {
public:
  virtual bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const = 0;
  virtual ~hittable() = default;
};

static bool near_zero(glm::dvec3& e) {
  const auto s = 1e-8;
  return (fabs(e.x) < s) && (fabs(e.y) < s) && (fabs(e.z) < s);
}

static glm::dvec3 random_unit() {
  double a = 2 * PI * rnd();
  double z = 1 - 2 * rnd();
  double r = std::sqrt(1 - z * z);
  return glm::dvec3(r * std::cos(a), r * std::sin(a), z);
}

// Lambertian Diffuse ----------------------------------------------------------
class lambertian : public material {
  glm::vec3 albedo;
public:
  lambertian(const glm::vec3& a) : albedo(a) {}
  bool scatter(const ray&, const hit_record& rec, scatter_pack& s) const override {
    glm::dvec3 scatter_dir = rec.normal + glm::normalize(random_unit());

    if (near_zero(scatter_dir)) scatter_dir = rec.normal;
    s.spec = ray(rec.p, scatter_dir);
    s.atten = albedo;
    s.is_specular = false;
    return true;
  }
};

// Diffuse area light ----------------------------------------------------------
class diffuse_light : public material {
  glm::vec3 emit_colour;
public:
  diffuse_light(const glm::vec3& c) : emit_colour(c) {}
  bool scatter(const ray&, const hit_record&, scatter_pack&) const override { return false; }
  glm::vec3 emitted() const override { return emit_colour; }
};

// Henyey–Greenstein Phase Function -------------------------------------------
class henyey_greenstein : public material {
  double g;       // asymmetry (‑1 .. 1)
  glm::vec3 albedo;  // single‑scattering albedo
public:
  henyey_greenstein(double asym, const glm::vec3& a) : g(asym), albedo(a) {}
  bool scatter(const ray&, const hit_record& rec, scatter_pack& s) const override {
    // Sample HG using inversion (stable for |g| < 0.99)
    double cos_theta;
    if (std::fabs(g) < 1e-3) cos_theta = 1 - 2 * rnd();
    else {
      double sq = (1 - g * g) / (1 - g + 2 * g * rnd());
      cos_theta = (1 + g * g - sq * sq) / (2 * g);
    }
    double sin_theta = std::sqrt(std::max(0.0, 1 - cos_theta * cos_theta));
    double phi = 2 * PI * rnd();
    glm::dvec3 w = rec.normal;                     // any arbitrary frame; normal is fine
    glm::dvec3 u = glm::normalize(glm::cross((std::fabs(w.x) > 0.1 ? glm::dvec3{ 0,1,0 } : glm::dvec3{ 1,0,0 }), w));
    glm::dvec3 v = glm::cross(w, u);
    glm::dvec3 dir = cos_theta * w + sin_theta * std::cos(phi) * u + sin_theta * std::sin(phi) * v;
    s.spec = ray(rec.p, normalize(dir));
    s.atten = albedo;
    s.is_specular = false;
    return true;
  }
};

// --- utilities ---
inline glm::dvec3 fresnel_schlick(double cosTheta, const glm::dvec3& F0) {
  return F0 + (glm::dvec3{ 1,1,1 } - F0) * pow(1.0 - cosTheta, 5.0);
}

// --- glass boundary ----------------------------------------------------------
class dielectric : public material {
  double eta;
public:
  dielectric(double index) : eta(index) {}
  bool scatter(const ray& r_in, const hit_record& rec, scatter_pack& s) const override {
    glm::dvec3 unit_d = glm::normalize(r_in.dir);
    double cosi = std::fmin(dot(-unit_d, rec.normal), 1.0);
    double etai_over_etat = rec.front ? 1.0 / eta : eta;

    double sint2 = etai_over_etat * etai_over_etat * (1 - cosi * cosi);
    bool cannot_refract = sint2 > 1.0;
    glm::dvec3 dir;
    if (cannot_refract || rnd() < fresnel_schlick(cosi, glm::dvec3{ 0.04,0.04,0.04 }).x)
      dir = reflect(unit_d, rec.normal);
    else
      dir = refract(unit_d, rec.normal, etai_over_etat);

    s.spec = ray(rec.p, dir);
    s.atten = { 1,1,1 };
    s.is_specular = true;
    return true;
  }
};

// --- random‑walk subsurface (homogeneous) ------------------------------------
struct MediumParams { glm::dvec3 sigma_s, sigma_a; double g; };

inline glm::dvec3 sample_hg(double g, const glm::dvec3& wo) {
  double xi = rnd();
  double cos_theta;
  if (std::abs(g) < 1e-3) cos_theta = 1 - 2 * xi;
  else {
    double sq = (1 - g * g) / (1 - g + 2 * g * xi);
    cos_theta = (1 + g * g - sq * sq) / (2 * g);
  }
  double sin_theta = std::sqrt(std::max(0.0, 1 - cos_theta * cos_theta));
  double phi = 2 * PI * rnd();

  // build local frame around wo
  glm::dvec3 w = glm::normalize(wo);
  glm::dvec3 u = glm::normalize(glm::cross((std::abs(w.x) > .1 ? glm::dvec3(0, 1, 0)
    : glm::dvec3(1, 0, 0)), w));
  glm::dvec3 v = glm::cross(w, u);

  return glm::normalize(cos_theta * w +
    sin_theta * std::cos(phi) * u +
    sin_theta * std::sin(phi) * v);
}

class random_walk_sss : public material {
  glm::dvec3 sigma_s, sigma_a;    // per‑channel
  double g;                        // HG asymmetry
  double eta;                      // IOR (≈1.3‑1.6 for organics)
  double radius;                   // object radius (needed to know when we exit)
  glm::dvec3 centre;
public:
  random_walk_sss(glm::dvec3 c, double R,
    glm::dvec3 sig_s,
    glm::dvec3 sig_a,
    double g_, double eta_)
    : sigma_s(sig_s * 100.0), sigma_a(sig_a * 100.0), g(g_),
    eta(eta_), radius(R), centre(c) { //sigma_s, sigma_a scaled by 100, 1 unit = 100 mm = 10 cm
  }

  bool scatter(const ray& r_in, const hit_record& rec, scatter_pack& s) const override
  {
    /* ---------- 1. Fresnel split -------------------------------------- */
    glm::dvec3 wo = glm::normalize(r_in.dir);
    glm::dvec3 n = rec.normal;
    double cos_i = std::clamp(dot(-wo, n), 0.0, 1.0);
    double eta_i = rec.front ? 1.0 : eta;
    double eta_t = rec.front ? eta : 1.0;
    double eta_rel = eta_i / eta_t;

    double R0 = (eta_rel - 1) * (eta_rel - 1) / ((eta_rel + 1) * (eta_rel + 1));
    double Fr = R0 + (1 - R0) * std::pow(1 - cos_i, 5);
    //Fr = std::clamp(Fr, 0.0, 0.25);
    Fr *= 0.35;

    if (rnd() < Fr) {                           // specular reflection
      glm::dvec3 refl = glm::reflect(wo, n);
      s.spec = ray(rec.p, refl);
      s.atten = { 1,1,1 };
      s.is_specular = true;
      return true;
    }

    /* ---------- 2. Refract into medium -------------------------------- */
    glm::dvec3 wi = glm::refract(wo, n, eta_rel);
    if (near_zero(wi)) return false;            // TIR (rare)

    /* ▼---------- 3. Pure random‑walk in homogeneous medium  ----------- */
    glm::dvec3 Tr(1, 1, 1);                      // path throughput
    glm::dvec3 pos = rec.p + 1e-4 * wi;        // push inside
    glm::dvec3 dir = wi;

    for (int bounce = 0; bounce < 128; ++bounce)
    {
      /* 3a. pick *spectral channel* to sample distance */
      glm::dvec3 sigma_t = sigma_s + sigma_a;          // RGB σ_t
      double sum_sigma = sigma_t.x + sigma_t.y + sigma_t.z;
      if (sum_sigma == 0) break;                       // vacuum (shouldn’t happen)

      double xi = rnd() * sum_sigma;
      int ch = (xi < sigma_t.x) ? 0 : (xi < sigma_t.x + sigma_t.y ? 1 : 2);
      double sigma_t_ch = (ch == 0 ? sigma_t.x : (ch == 1 ? sigma_t.y : sigma_t.z));

      /* 3b. sample free path from exp(−σ_t_ch t) */
      double t = -std::log(1 - rnd()) / sigma_t_ch;
      pos += dir * t;

      /* exit? */
      if (glm::length2(pos - centre) >= radius * radius) {
        glm::dvec3 n_exit = glm::normalize(pos - centre);
        //glm::dvec3 out = sample_hg(0.0, n_exit);      // cosine-weight
        glm::dvec3 out = sample_hg(g, n_exit);      // cosine-weight
        s.spec = ray(pos, out);
        s.atten = Tr;
        s.is_specular = false;
        return true;
      }

      /* 3c. weight for distance (only absorption!) */
      glm::dvec3 att = glm::exp(-sigma_a * t);
      Tr *= att * (sigma_s / sigma_t);                    // single‑scatter albedo

      /* 3d. Russian‑roulette after 3 bounces */
      if (bounce > 2) {
        double q = std::max({ Tr.x, Tr.y, Tr.z });
        if (rnd() > q) return false;
        Tr /= q;
      }

      /* 3e. scatter direction (HG) */
      dir = sample_hg(g, dir);
    }
    return false;                                         // too deep, terminate
  }

};


// Simple Objects --------------------------------------------------------------
class sphere : public hittable {
  glm::dvec3 centre; double radius; std::shared_ptr<material> mat_ptr;
public:
  sphere(glm::dvec3 c, double r, std::shared_ptr<material> m) : centre(c), radius(r), mat_ptr(m) {}
  bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
    glm::dvec3 oc = r.orig - centre;
    double a = glm::length2(r.dir);
    double half_b = dot(oc, r.dir);
    double c = glm::length2(oc) - radius * radius;
    double disc = half_b * half_b - a * c;
    if (disc < 0) return false;
    double sqrtd = std::sqrt(disc);
    double root = (-half_b - sqrtd) / a;
    if (root<t_min || root>t_max) { root = (-half_b + sqrtd) / a; if (root<t_min || root>t_max) return false; }
    rec.t = root;
    rec.p = r.at(rec.t);
    glm::dvec3 outward = (rec.p - centre) / radius;
    rec.set_face(r, outward);
    rec.mat = mat_ptr;
    return true;
  }
};

// Axis‑aligned rectangles for Cornell walls -----------------------------------
class xy_rect : public hittable {
  double x0, x1, y0, y1, k; glm::dvec3 normal; std::shared_ptr<material> mp;
public:
  xy_rect(double _x0, double _x1, double _y0, double _y1, double _k, const glm::dvec3& normal, std::shared_ptr<material> m)
    : x0(_x0), x1(_x1), y0(_y0), y1(_y1), k(_k), normal(normal), mp(m) {
  }
  bool hit(const ray& r, double t0, double t1, hit_record& rec) const override {
    double t = (k - r.orig.z) / r.dir.z;
    if (t<t0 || t>t1) return false;
    double x = r.orig.x + t * r.dir.x;
    double y = r.orig.y + t * r.dir.y;
    if (x<x0 || x>x1 || y<y0 || y>y1) return false;
    rec.t = t; rec.p = r.at(t);
    rec.set_face(r, normal);
    rec.mat = mp; return true;
  }
};
class xz_rect : public hittable {
  double x0, x1, z0, z1, k; glm::dvec3 normal; std::shared_ptr<material> mp;
public:
  xz_rect(double _x0, double _x1, double _z0, double _z1, double _k, const glm::dvec3& normal, std::shared_ptr<material> m)
    : x0(_x0), x1(_x1), z0(_z0), z1(_z1), k(_k), normal(normal), mp(m) {
  }
  bool hit(const ray& r, double t0, double t1, hit_record& rec) const override {
    double t = (k - r.orig.y) / r.dir.y;
    if (t<t0 || t>t1) return false;
    double x = r.orig.x + t * r.dir.x;
    double z = r.orig.z + t * r.dir.z;
    if (x<x0 || x>x1 || z<z0 || z>z1) return false;
    rec.t = t; rec.p = r.at(t);
    rec.set_face(r, normal);
    rec.mat = mp; return true;
  }
};
class yz_rect : public hittable {
  double y0, y1, z0, z1, k; glm::dvec3 normal; std::shared_ptr<material> mp;
public:
  yz_rect(double _y0, double _y1, double _z0, double _z1, double _k, const glm::dvec3& normal, std::shared_ptr<material> m)
    : y0(_y0), y1(_y1), z0(_z0), z1(_z1), k(_k), normal(normal), mp(m) {
  }
  bool hit(const ray& r, double t0, double t1, hit_record& rec) const override {
    double t = (k - r.orig.x) / r.dir.x;
    if (t<t0 || t>t1) return false;
    double y = r.orig.y + t * r.dir.y;
    double z = r.orig.z + t * r.dir.z;
    if (y<y0 || y>y1 || z<z0 || z>z1) return false;
    rec.t = t; rec.p = r.at(t);
    rec.set_face(r, normal);
    rec.mat = mp; return true;
  }
};

// World (a list) --------------------------------------------------------------
class hittable_list : public hittable {
  std::vector<std::shared_ptr<hittable>> objects;
public:
  void add(std::shared_ptr<hittable> obj) { objects.push_back(obj); }
  bool hit(const ray& r, double t_min, double t_max, hit_record& rec) const override {
    hit_record tmp; bool hit_any = false; double closest = t_max;
    for (const auto& obj : objects)
      if (obj->hit(r, t_min, closest, tmp)) { hit_any = true; closest = tmp.t; rec = tmp; }
    return hit_any;
  }
};

// Camera ----------------------------------------------------------------------
class camera {
  glm::dvec3 orig; glm::dvec3 lower_left; glm::dvec3 horiz, vert, u, v, w; double lens_rad;
public:
  camera(glm::dvec3 lookfrom, glm::dvec3 lookat, glm::dvec3 vup, double vfov, double aspect) {
    double theta = vfov * PI / 180; double h = std::tan(theta / 2);
    double viewport_h = 2 * h; double viewport_w = aspect * viewport_h;
    w = normalize(lookfrom - lookat);
    u = normalize(cross(vup, w));
    v = cross(w, u);
    orig = lookfrom;
    horiz = viewport_w * u;
    vert = viewport_h * v;
    lower_left = orig - horiz / 2.0 - vert / 2.0 - w;
  }
  ray get_ray(double s, double t) const {
    return ray(orig, lower_left + s * horiz + t * vert - orig);
  }
};

// Colour computation ----------------------------------------------------------
inline glm::vec3 trace(const ray& r, const glm::vec3& bg, const hittable& world, int depth) {
  if (depth <= 0) return { 0,0,0 };
  hit_record rec;
  if (!world.hit(r, 1e-4, INF, rec)) return bg;
  scatter_pack s;
  glm::vec3 emit = rec.mat->emitted();
  if (!rec.mat->scatter(r, rec, s)) return emit;
  return emit + s.atten * trace(s.spec, bg, world, depth - 1);
}

// Scene: 2×2×2 Cornell ---------------------------------------------------------
inline hittable_list cornell_box() {
  hittable_list world;
  auto red = std::make_shared<lambertian>(glm::vec3{ .65,.05,.05 });
  auto green = std::make_shared<lambertian>(glm::vec3{ .12,.45,.15 });
  auto white = std::make_shared<lambertian>(glm::vec3{ .73,.73,.73 });
  auto light = std::make_shared<diffuse_light>(glm::vec3{ 15,15,15 });

  // x = 2  (left wall)   normal  (-1,0,0)
  world.add(make_shared<yz_rect>(0, 3, 0, 3, 3, glm::dvec3(-1, 0, 0), green));

  // x = 0  (right wall)  normal  (+1,0,0)
  world.add(make_shared<yz_rect>(0, 3, 0, 3, 0, glm::dvec3( 1,0,0 ), red));

  // y = 0  (floor)       normal  (0,+1,0)
  world.add(make_shared<xz_rect>(0, 3, 0, 3, 0, glm::dvec3( 0,1,0 ), white));

  // y = 2  (ceiling)     normal  (0,-1,0)
  world.add(make_shared<xz_rect>(0, 3, 0, 3, 3, glm::dvec3( 0,-1,0 ), white));

  // z = 2  (back wall)   normal  (0,0,-1)
  world.add(make_shared<xy_rect>(0, 3, 0, 3, 3, glm::dvec3( 0,0,-1 ), white));

  // light patch in the ceiling – also faces down  (0,-1,0)
  world.add(make_shared<xz_rect>(1.5, 2.5, 1.5, 2.5, 2.999,
    glm::dvec3( 0,-1,0 ), light));

  // --- presets (RGB units: mm‑1 if you like, just relative) -------------------
  auto wax = std::make_shared<random_walk_sss>(
    glm::dvec3{ 0.5,1,1 }, 0.3,
    glm::dvec3{ 0.7, 0.7, 0.7 },   // sigma_s
    glm::dvec3{ 0.001,0.002,0.002 },// sigma_a
    0.8, 1.44);

  auto jade = std::make_shared<random_walk_sss>(
    glm::dvec3{ 1.5,1,1 }, 0.3,
    glm::dvec3{ 1.5, 2.0, 2.6 },
    glm::dvec3{ 0.01, 0.02, 0.06 },
    0.8, 1.45);

  auto skin = std::make_shared<random_walk_sss>(
    glm::dvec3{ 2.5,1,1 }, 0.3,
    glm::dvec3{ 0.74, 0.88, 1.01 },
    glm::dvec3{ 0.032,0.17, 0.48 },  // more absorption in blue/green, little in red
    0.9, 1.4);

  // geometry
  world.add(std::make_shared<sphere>(glm::dvec3{ 0.5,1,1 }, 0.3, wax));
  world.add(std::make_shared<sphere>(glm::dvec3{ 1.5,1,1 }, 0.3, jade));
  world.add(std::make_shared<sphere>(glm::dvec3{ 2.5,1,1 }, 0.3, skin));

  
  return world;
}