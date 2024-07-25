
#include <cstdlib>
#include <stdio.h>

#include "../ext/glad/include/glad.h"
#include "../ext/glfw/include/GLFW/glfw3.h"
#include "utils/utils.h"
#include "window/windowContext.h"

#include "raytracer/geometry/ray.h"
#include "raytracer/geometry/vec3.h"

#include "raytracer/camera.h"
#include "raytracer/worlds.h"

#include <time.h>

vec3 rayColor(const ray& camera_ray, World* world, i32 depth, randState* random_state){
  if(depth <= 0) return vec3(0, 0, 0);
 
  // World Objects Collisions
  hit_record rec;  
  bool hitted = world->collider->hit(camera_ray, 0.001, INF, rec);
  if (hitted) {    
    ray scattered_ray;
    vec3 attenuation; 
    bool scattered = rec.mat_ptr->scatter(camera_ray, rec, attenuation, scattered_ray, random_state);
    if(scattered){
      return attenuation * rayColor(scattered_ray, world, depth - 1, random_state); 
    } 
  }

  vec3 unit_direction = normalize(camera_ray.direction());
  f64 t               = 0.5 * (unit_direction.y() + 1.0);
  vec3 pixel_color    = (1.0 - t) * world->sky_color1 + t * world->sky_color2;
  return pixel_color;
}

void rayTrace(u8 *texture_data, i32 width, i32 height, i32 scanline, World* world, randState* random_state) {
  u32 j = scanline; 
  for (int i = 0; i < width; i++) {
    vec3 col(0, 0, 0); 
    // Ray Tracing
    for (int s = 0; s < world->pixel_samples; s++) {
      f64 u = f64(i + RANDOM_UNIFORM(random_state))/ f64(width);
      f64 v = f64(j + RANDOM_UNIFORM(random_state))/ f64(height);
      ray r = (world->camera)->get_ray(u, v, random_state);
      col = col + rayColor(r, world, world->ray_max_depth, random_state);
    }
    col = col / f64(world->pixel_samples);
    col = vec3(sqrt(col.x()), sqrt(col.y()), sqrt(col.z()));

    // Write texture data
    int index               = (j*width + i) * 4;
    texture_data[index]     = (u8)(255.0f * col.x());
    texture_data[index + 1] = (u8)(255.0f * col.y());
    texture_data[index + 2] = (u8)(255.0f * col.z());
    texture_data[index + 3] = 255;
  }
}

void fullRayTrace(u8 *texture_data, i32 width, i32 height, World* world, randState* random_state) {
  for(i32 scanline = 0; scanline < height; scanline++){
    rayTrace(texture_data, width, height, scanline, world, random_state);
  }
}

int main() {  
  //------------------------------------
  // Init Window and Renderer
  //------------------------------------
  f32 aspect_ratio  = 16.0 / 9.0;
  i32 width         = 1200;
  i32 height        = (i32)(width / aspect_ratio);
  height = (height < 1) ? 1 : height;
  const char *title = "Luminara"; 

  i32 pixel_samples = 10;
  i32 ray_max_depth = 20;  
  
  WindowContext windowContext;
  windowContext.glfw_window = initWindowGLFW(width, height, title);
  initRenderer(&windowContext.renderer, width, height);
  
  //------------------------------------
  // World + Camera + Materials
  //------------------------------------
  std::random_device rd;
  std::mt19937 gen(rd());
  
  // World *world = simple_world(aspect_ratio);
  World *world = book_cover_world(aspect_ratio, &gen);
 
  world->pixel_samples = pixel_samples;
  world->ray_max_depth = ray_max_depth;
  
  //------------------------------------
  // Prepare Render Texture and run RayTracing
  //------------------------------------  
  u8 *texture_data = (u8 *) malloc(width * height * 4);
  memset(texture_data, 0, width * height * 4);

  // Comment to see the render in real time
  printf("RayTracing...\n");
  clock_t start, stop;
  start = clock();  
  fullRayTrace(texture_data, width, height, world, &gen);
  stop = clock();
  f64 timer_seconds = ((f64)(stop - start)) / CLOCKS_PER_SEC; 
  printf("Took %f s\n", timer_seconds);
  
  // Render loop
  i32 scanline = 0;
  while (!glfwWindowShouldClose(windowContext.glfw_window)) {
    glClear(GL_COLOR_BUFFER_BIT);

    // Uncomment to see the render in real time
    // if (scanline < height) {
      // rayTrace(texture_data, width, height, scanline, world, &gen);
      // scanline++;
    // }

    // Render pixel data into a OpenGL texture
    glBindTexture(GL_TEXTURE_2D, windowContext.renderer.texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, texture_data);

    glUseProgram(windowContext.renderer.shaderProgram);
    glBindTexture(GL_TEXTURE_2D, windowContext.renderer.texture);
    glBindVertexArray(windowContext.renderer.VAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    glfwSwapBuffers(windowContext.glfw_window);
    glfwPollEvents();
  }

  // Free Texture
  free(texture_data);
  free(world);

  // Save result in the path
  saveRenderTexture(windowContext.renderer.texture, width, height, "raytraced_image.png");

  // Terminate Window Context data structures
  terminateRenderer(&windowContext.renderer);
  terminateWindowGLFW(windowContext.glfw_window);
  return 0;
}
