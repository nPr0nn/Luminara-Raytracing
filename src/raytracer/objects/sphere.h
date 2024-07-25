#ifndef SPHEREH
#define SPHEREH

#include "hittable.h"

class sphere : public hittable {
public:
  vec3 center;
  f32 radius;
  material *mat_ptr;

  DEVICE sphere() {}
  DEVICE sphere(vec3 cen, f32 r, material *m)
      : center(cen), radius(r), mat_ptr(m){};
  DEVICE virtual bool hit(const ray &r, f32 tmin, f32 tmax,
                              hit_record &rec) const;
};

DEVICE bool sphere::hit(const ray &r, f32 t_min, f32 t_max,
                            hit_record &rec) const {

  vec3 oc = r.origin() - center;
  f32 a = r.direction().norm_squared();
  f32 h = dot(oc, r.direction());
  f32 c = oc.norm_squared() - radius * radius;
  f32 discriminant = h * h - a * c;

  if (discriminant <= 0)
    return false;

  f32 value = (-h - sqrt(discriminant)) / a;
  if (!INTERVAL_SURROUND(t_min, t_max, value)) {
    value = (-h + sqrt(discriminant)) / a;
    if (!INTERVAL_SURROUND(t_min, t_max, value))
      return false;
  }

  rec.t = value;
  rec.p = r.at(rec.t);
  rec.normal = (rec.p - center) / radius;
  rec.mat_ptr = mat_ptr;

  return true;
}

#endif
