#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "hittable.h"

// Triangle primitive
class triangle : public hittable {
public:
  vec3 vertices[3];
  vec3 normals[3];
  material *mat_ptr;
  bool back_culling;

  DEVICE triangle() {}
  DEVICE triangle(vec3 v[3], vec3 n[3], material *m, bool b = true) {    
    for(int i = 0; i < 3; i++)  vertices[i] = v[i];
    for(int i = 0; i < 3; i++)  normals[i]  = n[i];
    mat_ptr  = m;
    back_culling = b;
  }
  DEVICE virtual bool hit(const ray &r, f32 tmin, f32 tmax, hit_record &rec) const;
};

DEVICE inline bool triangle::hit(const ray &r, f32 t_min, f32 t_max, hit_record &rec) const {

  // Möller –Trumbore intersection algorithm for triangle
  const vec3 &p0 = vertices[0];
  const vec3 &p1 = vertices[1];
  const vec3 &p2 = vertices[2];
  
  // Find vectors for two edges sharing p0
  vec3 e1 = p1 - p0;
  vec3 e2 = p2 - p0;
  
  // Begin calculating determinant - also used to calculate u parameter
  vec3 P = cross(r.direction(), e2);
  
  // if determinant is near zero, ray lies in plane of triangle
  f32 det = dot(e1, P);
  if (det < EPSILON && back_culling) return false;  

  f32 inv_det = 1.f / det;

  // calculate distance from V1 to ray origin
  vec3 T = r.origin() - p0;

  // Calculate u parameter and test bound
  f32 u = dot(T, P) * inv_det;
  // The intersection lies outside of the triangle
  if (u < 0.f || u > 1.f) {
    return false;
  }

  // Prepare to test v parameter
  vec3 Q = cross(T, e1);

  // Calculate V parameter and test bound
  f32 v = dot(r.direction(), Q) * inv_det;
  // The intersection lies outside of the triangle
  if (v < 0.f || u + v > 1.f) return false;

  f32 t = dot(e2, Q) * inv_det;

  const vec3 &n0 = normals[0];
  const vec3 &n1 = normals[1];
  const vec3 &n2 = normals[2];

  // Interpolate to find normal
  vec3 n = (1 - u - v) * n0 + u * n1 + v * n2;

  rec.t         = t;
  rec.p         = r.at(rec.t);
  rec.normal    = normalize(n);
  rec.mat_ptr   = mat_ptr;

  return true;
}

#endif /* TRIANGLE_H */
