#ifndef WORLD

#include "objects/hittable.h"
#include "objects/sphere.h"
#include "objects/triangle.h"

#include "materials.h"
#include "camera.h"

typedef struct World{
  // Objects
  hittable** objects;
  hittable* collider;
  u32 objects_count; 

  // Camera
  Camera* camera;
  
  // Ray Variables
  i32 pixel_samples;
  i32 ray_max_depth;
  
  // Sky Box
  vec3 sky_color1;
  vec3 sky_color2;

} World;

//--------------------------------------------------------------------------------------------------
// World 1 
 
inline World* simple_world(f32 aspect_ratio){
  World* world    = (World*) malloc(sizeof(World));
 
  world->objects_count = 20;
  world->objects       = new hittable*[20];
 
  // ground
  u32 i = 0;
  world->objects[i++]    = new sphere(vec3(0, -100.5, -1), 100, new lambertian(vec3(0.1, 0.5, 0.1)));
   
  // spheres
  world->objects[i++]    = new sphere(vec3(0, 0, 0), 0.5, new lambertian(vec3(0.1, 0.2, 0.5)));  
  world->objects[i++]    = new sphere(vec3(1, 0, 0), 0.5, new metal(vec3(0.8, 0.6, 0.2), 0.0));
  world->objects[i++]    = new sphere(vec3(-1, 0, 0), 0.5, new dielectric(1.5)); 

  // triangle 1
  vec3 v[3] = {vec3(0, 1, 0.5), vec3(1, 0, 0.5), vec3(-1, 0, 0.5)};
  vec3 n[3] = {cross(v[0], v[1]), cross(v[0], v[1]), cross(v[0], v[1])};
  world->objects[i++]    = new triangle(v, n, new dielectric(1.5), true);

  // triangle 2
  vec3 v2[3] = {vec3(0.5, 1, 1), vec3(1.5, 0, 1), vec3(-0.5, 0, 1) };
  vec3 n2[3] = {cross(v[0], v[1]), cross(v[0], v[1]), cross(v[0], v[1])}; 
  world->objects[i++]    = new triangle(v2, n2, new lambertian(vec3(0.1, 0.2, 0.5)), true);
 
  // Collider and Sky
  world->objects_count = i;
  world->collider      = new hittable_list(world->objects, i);  
  world->sky_color1    = vec3(1, 0.9, 1);
  world->sky_color2    = vec3(0.4, 0.5, 1.0);
 
  // Camera
  vec3 lookfrom     = vec3(0, 0, -10);
  vec3 lookat       = vec3(0, 0, 0);
  vec3 vup          = vec3(0, 1, 0); 
  f64 vfov          = 20;
  f64 aperture      = 0.1;
  f64 focus_dist    = 10.0;
  world->camera     = new Camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, focus_dist);  
  
  return world;
}

//--------------------------------------------------------------------------------------------------
// World 2

inline World* book_cover_world(f32 aspect_ratio, randState* random_state){
  World* world         = (World*) malloc(sizeof(World)); 
  
  world->objects    = new hittable*[500]; 
  world->objects[0] = new sphere(vec3(0, -1000, 0), 1000, new lambertian(vec3(0.5, 0.5, 0.5)));
  
  i32 i = 1;
  for (i32 a = -11; a < 11; a++) {
    for (i32 b = -11; b < 11; b++) {
      f64 choose_mat = RANDOM_UNIFORM(random_state);
      vec3 center(a + 0.9 * RANDOM_UNIFORM(random_state), 0.2, b + 0.9 * RANDOM_UNIFORM(random_state));
      
      if ((center - vec3(4, 0.2, 0)).norm() > 0.9) { 
        // Diffuse 
        if (choose_mat < 0.8) {
          vec3 albedo = random_vec3(0, 1, random_state) * random_vec3(0, 1, random_state);
          world->objects[i++] = new sphere(center, 0.2, new lambertian(albedo));
        }
      
        // Metal
        else if (choose_mat < 0.95) {
          vec3 albedo = random_vec3(0.5, 1, random_state);
          f64 fuzz    = RANDOM_IN_RANGE(0, 0.5, random_state); 
          world->objects[i++]   = new sphere(center, 0.2, new metal(albedo, fuzz));
        }
       
        // Glass
        else {
          world->objects[i++] = new sphere(center, 0.2, new dielectric(1.5));
        }
      }
    }
  }

  // more spheres
  world->objects[i++] = new sphere(vec3(0, 1, 0), 1.0, new dielectric(1.5));
  world->objects[i++] = new sphere(vec3(-4, 1, 0), 1.0, new lambertian(vec3(0.4, 0.2, 0.1)));
  world->objects[i++] = new sphere(vec3(4, 1, 0), 1.0, new metal(vec3(0.7, 0.6, 0.5), 0.0));
  world->objects_count = i;

  // Collider and Sky
  world->collider = new hittable_list(world->objects, i);
  world->sky_color1 = vec3(1, 1, 1);
  world->sky_color2 = vec3(0.5, 0.7, 1.0);

  // Camera
  vec3 lookfrom     = vec3(13, 2, 3);
  vec3 lookat       = vec3(0, 0, 0);
  vec3 vup          = vec3(0, 1, 0); 
  f64 vfov          = 20;
  f64 aperture      = 0.1;
  f64 focus_dist    = 10.0;
  world->camera     = new Camera(lookfrom, lookat, vup, vfov, aspect_ratio, aperture, focus_dist);  
  return world;
}

#endif // !WORLD
