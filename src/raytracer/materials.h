#ifndef MATERIALH
#define MATERIALH

#include "geometry/ray.h"
#include "objects/hittable.h"

class material {
public:
  DEVICE virtual bool scatter(const ray &r_in, const hit_record &rec,
                              vec3 &attenuation, ray &scattered,
                              randState *local_rand_state) const = 0;
};

class lambertian : public material {
public:
  vec3 albedo;

  DEVICE lambertian(const vec3 &a) : albedo(a) {}
  DEVICE virtual bool scatter(const ray &r_in, const hit_record &rec,
                              vec3 &attenuation, ray &scattered,
                              randState *local_rand_state) const {
    vec3 target = rec.p + rec.normal + random_in_unit_sphere(local_rand_state);
    scattered = ray(rec.p, target - rec.p);
    attenuation = albedo;
    return true;
  }
};

class metal : public material {
public:
  vec3 albedo;
  f32 fuzz;

  DEVICE metal(const vec3 &a, f32 f) : albedo(a) {
    if (f < 1)
      fuzz = f;
    else
      fuzz = 1;
  }
  DEVICE virtual bool scatter(const ray &r_in, const hit_record &rec,
                              vec3 &attenuation, ray &scattered,
                              randState *local_rand_state) const {
    vec3 reflected = reflect(normalize(r_in.direction()), rec.normal);
    scattered =
        ray(rec.p, reflected + fuzz * random_in_unit_sphere(local_rand_state));
    attenuation = albedo;
    return (dot(scattered.direction(), rec.normal) > 0.0f);
  }
};

class dielectric : public material {
public:
  f32 ref_idx;

  DEVICE dielectric(f32 ri) : ref_idx(ri) {}
  DEVICE virtual bool scatter(const ray &r_in, const hit_record &rec,
                              vec3 &attenuation, ray &scattered,
                              randState *local_rand_state) const {
    vec3 outward_normal;
    vec3 reflected = reflect(r_in.direction(), rec.normal);
    f32 ni_over_nt;
    attenuation = vec3(1.0, 1.0, 1.0);
    vec3 refracted;
    f32 reflect_prob;
    f32 cosine;
    if (dot(r_in.direction(), rec.normal) > 0.0f) {
      outward_normal = -rec.normal;
      ni_over_nt = ref_idx;
      cosine = dot(r_in.direction(), rec.normal) / r_in.direction().norm();
      cosine = sqrt(1.0f - ref_idx * ref_idx * (1 - cosine * cosine));
    } else {
      outward_normal = rec.normal;
      ni_over_nt = 1.0f / ref_idx;
      cosine = -dot(r_in.direction(), rec.normal) / r_in.direction().norm();
    }
    if (refract(r_in.direction(), outward_normal, ni_over_nt, refracted))
      reflect_prob = schlick(cosine, ref_idx);
    else
      reflect_prob = 1.0f;
    if (RANDOM_UNIFORM(local_rand_state) < reflect_prob)
      scattered = ray(rec.p, reflected);
    else
      scattered = ray(rec.p, refracted);
    return true;
  }
};
#endif
