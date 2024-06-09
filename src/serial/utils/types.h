
#ifndef TYPES_H
#define TYPES_H

#include <inttypes.h>
#include <stdbool.h>
#include <math.h>

typedef uint8_t   u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t    i8;
typedef int16_t  i16;
typedef int32_t  i32;
typedef int64_t  i64;

typedef float    f32;
typedef double   f64;

typedef struct t_color{
  u8 r;
  u8 g;
  u8 b;
  u8 a;
} t_color;

#define rgb(r, g, b)     (t_color) {r, g, b, 255}
#define rgba(r, g, b, a) (t_color) {r, g, b, a}

#endif
