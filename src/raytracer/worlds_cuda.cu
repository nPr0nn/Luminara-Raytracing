
#include "objects/hittable.h"
#include "objects/sphere.h"
#include "objects/triangle.h"

#include "camera.h"
#include "materials.h"

typedef struct World {
  // Objects
  hittable **objects;  // d_list
  hittable **collider; // d_world

  // Camera
  Camera **camera;
 
  i32 pixel_samples;
  i32 ray_max_depth;
  
  // Sky Box
  vec3 sky_color1;
  vec3 sky_color2;
} World;

#define RND (curand_uniform(&local_rand_state))

//--------------------------------------------------------------------------------------------------
// World 1

__global__ void simple_world(hittable **d_list, hittable **d_world, Camera **d_camera, f32 aspect_ratio){
  if (threadIdx.x == 0 && blockIdx.x == 0) {
    d_list[0] = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.1, 0.5, 0.1)));
 
    i32 i = 1;
    // spheres
    d_list[i++] = new sphere(vec3(0, 0, 0), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));  
    d_list[i++] = new sphere(vec3(1, 0, 0), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
    d_list[i++] = new sphere(vec3(-1, 0, 0), 0.5, new dielectric(1.5));  

    // triangle 1
    vec3 v[3] = {vec3(0, 1, 0.5), vec3(1, 0, 0.5), vec3(-1, 0, 0.5)};
    vec3 n[3] = {cross(v[0], v[1]), cross(v[0], v[1]), cross(v[0], v[1])};
    d_list[i++]    = new triangle(v, n, new dielectric(1.5), true);

    // triangle 2
    vec3 v2[3] = {vec3(0.5, 1, 1), vec3(1.5, 0, 1), vec3(-0.5, 0, 1) };
    vec3 n2[3] = {cross(v[0], v[1]), cross(v[0], v[1]), cross(v[0], v[1])}; 
    d_list[i++]    = new triangle(v2, n2, new lambertian(vec3(0.1, 0.2, 0.5)), true);
 
    *d_world = new hittable_list(d_list, i);

    // Camera
    vec3 lookfrom     = vec3(-4, 3, -10); 
    vec3 lookat       = vec3(0, 0, 0);
    vec3 vup          = vec3(0, 1, 0);
    
    f32 vfov          = 20;
    f32 aperture      = 0.1;
    f32 focus_dist    = 10.0;
 
    *d_camera = new Camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, focus_dist); 
  }
}

//--------------------------------------------------------------------------------------------------
// World 2
__global__ void book_cover_world(hittable **d_list, hittable **d_world, Camera **d_camera, f32 aspect_ratio, randState *rand_state) {

  if (threadIdx.x == 0 && blockIdx.x == 0) {
    randState local_rand_state = *rand_state;

    d_list[0] = new sphere(vec3(0, -1000.0, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));

    i32 i = 1;
    for (i32 a = -11; a < 11; a++) {
      for (i32 b = -11; b < 11; b++) {
        f32 choose_mat = RND;
        vec3 center(a + RND, 0.2, b + RND);
        if (choose_mat < 0.8f) {
          d_list[i++] =
              new sphere(center, 0.2, new lambertian(vec3(RND * RND, RND * RND, RND * RND)));
        } else if (choose_mat < 0.95f) {
          d_list[i++] = new sphere(center, 0.2,
              new metal(vec3(0.5f * (1.0f + RND), 0.5f * (1.0f + RND),
                             0.5f * (1.0f + RND)),
                        0.5f * RND));
        } else {
          d_list[i++] = new sphere(center, 0.2, new dielectric(1.5));
        }
      }
    }
    d_list[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
    d_list[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
    d_list[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
    *rand_state = local_rand_state;
    *d_world = new hittable_list(d_list, i);

    // Camera
    vec3 lookfrom     = vec3(13, 2, 3); 
    vec3 lookat       = vec3(0, 0, 0);
    vec3 vup          = vec3(0, 1, 0);
    
    f32 vfov          = 20;
    f32 aperture      = 0.1;
    f32 focus_dist    = 10.0;
 
    *d_camera = new Camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, focus_dist);
    
  }
}

__global__ void free_world(hittable **d_list, hittable **d_world, Camera **d_camera) {
  for (i32 i = 0; i < WORLD_SPACE; i++) {
    delete ((sphere *)d_list[i])->mat_ptr;
    delete d_list[i];
  }
  delete *d_world;
  delete *d_camera;
}
