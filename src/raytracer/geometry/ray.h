
#ifndef RAYH
#define RAYH

#include "vec3.h"

class ray {
public:
  vec3 _origin;
  vec3 _direction;

  DEVICE ray() {}
  DEVICE ray(const vec3 &a, const vec3 &b) {
    _origin = a;
    _direction = b;
  }

  DEVICE vec3 origin() const { return _origin; }
  DEVICE vec3 direction() const { return _direction; }
  DEVICE vec3 at(f32 t) const { return _origin + t * _direction; }
};

#endif
