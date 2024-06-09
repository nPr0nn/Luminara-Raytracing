#ifndef VEC3_H
#define VEC3_H

#include <math.h>
#include <stdio.h>

#include "types.h"

typedef struct {
    f64 e[3];
} vec3;

vec3 make_vec3(f64 e0, f64 e1, f64 e2) {
    vec3 v;
    v.e[0] = e0;
    v.e[1] = e1;
    v.e[2] = e2;
    return v;
}

#endif // VEC3_H
