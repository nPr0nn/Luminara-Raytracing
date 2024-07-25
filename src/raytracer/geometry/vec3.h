#ifndef VEC3H
#define VEC3H

#include "../../utils/utils.h"
#include <math.h>

class vec3 {
public:
  f32 e[3];

  HOST DEVICE vec3() {}
  HOST DEVICE vec3(f32 e0, f32 e1, f32 e2) {
    e[0] = e0;
    e[1] = e1;
    e[2] = e2;
  }
  HOST DEVICE inline f32 x() const { return e[0]; }
  HOST DEVICE inline f32 y() const { return e[1]; }
  HOST DEVICE inline f32 z() const { return e[2]; }
  HOST DEVICE inline f32 r() const { return e[0]; }
  HOST DEVICE inline f32 g() const { return e[1]; }
  HOST DEVICE inline f32 b() const { return e[2]; }

  HOST DEVICE inline vec3 operator-() const {
    return vec3(-e[0], -e[1], -e[2]);
  }
  HOST DEVICE inline f32 norm() const {
    return sqrt(e[0] * e[0] + e[1] * e[1] + e[2] * e[2]);
  }
  HOST DEVICE inline f32 norm_squared() const {
    return e[0] * e[0] + e[1] * e[1] + e[2] * e[2];
  }
};

HOST DEVICE void inline print_vec3(vec3 v){
  printf("(%f, %f, %f)\n", v.x(), v.y(), v.z());

}
//-----------------------------------------------------------------------------------
// Vector Algebra Operations
HOST DEVICE inline vec3 operator+(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] + v2.e[0], v1.e[1] + v2.e[1], v1.e[2] + v2.e[2]);
}

HOST DEVICE inline vec3 operator-(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] - v2.e[0], v1.e[1] - v2.e[1], v1.e[2] - v2.e[2]);
}

HOST DEVICE inline vec3 operator*(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] * v2.e[0], v1.e[1] * v2.e[1], v1.e[2] * v2.e[2]);
}

HOST DEVICE inline vec3 operator/(const vec3 &v1, const vec3 &v2) {
  return vec3(v1.e[0] / v2.e[0], v1.e[1] / v2.e[1], v1.e[2] / v2.e[2]);
}

HOST DEVICE inline vec3 operator*(f32 t, const vec3 &v) {
  return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

HOST DEVICE inline vec3 operator/(vec3 v, f32 t) {
  return vec3(v.e[0] / t, v.e[1] / t, v.e[2] / t);
}

HOST DEVICE inline vec3 operator*(const vec3 &v, f32 t) {
  return vec3(t * v.e[0], t * v.e[1], t * v.e[2]);
}

HOST DEVICE inline vec3 normalize(vec3 v) { return v / v.norm(); }

HOST DEVICE inline f32 dot(const vec3 &v1, const vec3 &v2) {
  return v1.e[0] * v2.e[0] + v1.e[1] * v2.e[1] + v1.e[2] * v2.e[2];
}

HOST DEVICE inline vec3 cross(const vec3 &v1, const vec3 &v2) {
  return vec3((v1.e[1] * v2.e[2] - v1.e[2] * v2.e[1]),
              (-(v1.e[0] * v2.e[2] - v1.e[2] * v2.e[0])),
              (v1.e[0] * v2.e[1] - v1.e[1] * v2.e[0]));
}

//-----------------------------------------------------------------------------------
// Vectors Surfaces Interactions Functions
DEVICE inline vec3 reflect(const vec3 &v, const vec3 &n) {
  return v - 2.0f * dot(v, n) * n;
}

DEVICE inline bool refract(const vec3 &v, const vec3 &n, f32 ni_over_nt,
                           vec3 &refracted) {
  vec3 uv = normalize(v);
  f32 dt = dot(uv, n);
  f32 discriminant = 1.0f - ni_over_nt * ni_over_nt * (1 - dt * dt);

  if (discriminant <= 0)
    return false;

  refracted = ni_over_nt * (uv - n * dt) - n * sqrt(discriminant);
  return true;
}

DEVICE inline f32 schlick(f32 cosine, f32 ref_idx) {
  f32 r0 = (1.0f - ref_idx) / (1.0f + ref_idx);
  r0 = r0 * r0;
  return r0 + (1.0f - r0) * pow((1.0f - cosine), 5.0f);
}

//-----------------------------------------------------------------------------------
// Random Vector Operations and Sampling
DEVICE inline vec3 random_vec3(f32 min, f32 max, randState *local_rand_state) {
  f32 x = RANDOM_IN_RANGE(min, max, local_rand_state);
  f32 y = RANDOM_IN_RANGE(min, max, local_rand_state);
  f32 z = RANDOM_IN_RANGE(min, max, local_rand_state);
  return vec3(x, y, z);
}

DEVICE inline vec3 random_in_unit_square(randState *local_rand_state) {
  vec3 rand_vec = random_vec3(-0.5, 0.5, local_rand_state);
  rand_vec.e[2] = 0.0f;
  return rand_vec;
}

DEVICE inline vec3 random_in_unit_disk(randState *local_rand_state) {
  while (true) {
    vec3 rand_vec = random_vec3(-1, 1, local_rand_state);
    rand_vec.e[2] = 0.0f;
    if (rand_vec.norm_squared() < 1)
      return rand_vec;
  }
}

DEVICE inline vec3 random_in_unit_sphere(randState *local_rand_state) {
  while (true) {
    vec3 rand_vec = random_vec3(-1, 1, local_rand_state);
    if (rand_vec.norm_squared() < 1)
      return rand_vec;
  }
}

DEVICE inline vec3 random_unit_vec3(randState *local_rand_state) {
  return normalize(random_in_unit_sphere(local_rand_state));
}

DEVICE inline vec3 random_hemisphere_vec3(const vec3 &normal,
                                          randState *local_rand_state) {
  vec3 rand_in_unit_sphere = random_unit_vec3(local_rand_state);
  if (dot(rand_in_unit_sphere, normal) > 0.0)
    return rand_in_unit_sphere;
  return -rand_in_unit_sphere;
}

#endif
