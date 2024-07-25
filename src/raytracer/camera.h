#ifndef CAMERAH
#define CAMERAH

#include "geometry/ray.h"

class Camera {
public:
  vec3 origin;
  vec3 lower_left_corner;
  vec3 horizontal;
  vec3 vertical;
  vec3 u, v, w;
  f32 lens_radius;

  DEVICE Camera(vec3 lookfrom, vec3 lookat, vec3 vup, f32 vfov, f32 aspect, f32 aperture, f32 focus_dist) {
    lens_radius = aperture / 2.0f;
    f32 theta = DEG2RAD(vfov);
    f32 half_height = tan(theta / 2.0f);
    f32 half_width = aspect * half_height;
    origin = lookfrom;
    w = normalize(lookfrom - lookat);
    u = normalize(cross(vup, w));
    v = cross(w, u);
    lower_left_corner = origin - half_width * focus_dist * u -
                        half_height * focus_dist * v - focus_dist * w;
    horizontal = 2.0f * half_width * focus_dist * u;
    vertical = 2.0f * half_height * focus_dist * v;
  }

  DEVICE ray get_ray(f32 s, f32 t, randState *local_rand_state) {
    vec3 rd = lens_radius * random_in_unit_disk(local_rand_state);
    vec3 offset = u * rd.x() + v * rd.y();
    return ray(origin + offset, lower_left_corner + s * horizontal +
                                    t * vertical - origin - offset);
  }
};

#endif
