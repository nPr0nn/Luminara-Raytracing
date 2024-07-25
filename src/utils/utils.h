#ifndef UTILS_H
#define UTILS_H

#include <math.h>
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

#include "types.h"
#include "logs.h"

#define WORLD_SPACE 1000 

// Useful math macros
#define INF 3.402823466e+38
#define EPSILON 1e-6
#define PI  3.1415926535897932385
#define DEG2RAD(x) ((x) * (PI / 180.0))
#define RAD2DEG(x) ((x) * (180.0 / PI))
#define MAX(x, y) (x > y ? x : y)
#define MIN(x, y) (x < y ? x : y)
#define pow2(x) (x*x)

// Useful functions on the interval [min_value, max_value]
#define INTERVAL_SIZE(min_value, max_value) (max_value - min_value)
#define INTERVAL_CENTER(min_value, max_value) (min_value + (INTERVAL_SIZE(min_value, max_value) / 2.0))
#define INTERVAL_CONTAIN(min_value, max_value, value) (min_value <= value && value <= max_value)
#define INTERVAL_SURROUND(min_value, max_value, value) (min_value < value && value < max_value)
#define INTERVAL_CLAMP(min_value, max_value, value) MAX(MIN(max_value, value), min_value)

// Macros to account for CUDA code 
#ifdef __CUDACC__
#define DEVICE __device__
#define HOST __host__
#include <curand_kernel.h>
typedef curandState randState;
#define RANDOM_UNIFORM(curandState) curand_uniform(curandState)
#else
#define DEVICE
#define HOST

#ifdef __cplusplus
#include <random>
typedef std::mt19937 randState;
static std::uniform_real_distribution<float> distribution(0.0f, 1.0f);
#define RANDOM_UNIFORM(state) distribution(*state)
#endif

#endif

#define RANDOM_IN_RANGE(min, max, state) ((min) + ((max) - (min)) * RANDOM_UNIFORM(state))

// Gamma Correction for colors
HOST DEVICE inline f32 gamma_correction(f32 linear_component){
  if (linear_component > 0) return sqrt(linear_component);
  return 0;
}

#endif
