#ifndef HITABLEH
#define HITABLEH

#include "../geometry/ray.h"

class material;
struct hit_record {
  f32 t;
  vec3 p;
  vec3 normal;
  material *mat_ptr;
};

class hittable {
public:
  DEVICE virtual bool hit(const ray &r, f32 t_min, f32 t_max,
                          hit_record &rec) const = 0;
};

class hittable_list : public hittable {
public:
  hittable **list;
  u32 list_size;

  DEVICE hittable_list() {}
  DEVICE hittable_list(hittable **l, u32 n) {
    list = l;
    list_size = n;
  }
  DEVICE virtual bool hit(const ray &r, f32 tmin, f32 tmax,
                          hit_record &rec) const;
};

DEVICE inline bool hittable_list::hit(const ray &r, f32 t_min, f32 t_max,
                               hit_record &rec) const {
  hit_record temp_rec;
  bool hit_anything = false;
  f32 closest_so_far = t_max;

  for (u32 i = 0; i < list_size; i++) {
    if (list[i]->hit(r, t_min, closest_so_far, temp_rec)) {
      hit_anything = true;
      closest_so_far = temp_rec.t;
      rec = temp_rec;
    }
  }
  return hit_anything;
}

#endif
