
#include "../ext/glad/include/glad.h"
#include "../ext/glfw/include/GLFW/glfw3.h"

#include "raytracer/objects/hittable.h"
#include "utils/utils.h"
#include "window/windowContext.h"

#include <nvToolsExt.h>

#include "raytracer/geometry/ray.h"
#include "raytracer/geometry/vec3.h"
#include "raytracer/camera.h"

#include "raytracer/worlds_cuda.cu"

#include <curand_kernel.h>
#include <time.h>

// limited version of checkCudaErrors
#define checkCudaErrors(val) check_cuda((val), #val, __FILE__, __LINE__)
void inline check_cuda(cudaError_t error, const char* func, const char* filename, i32 line) {
  if(error != cudaSuccess) {
    fprintf(stderr, "Error: %s:%d: %s: %s\n", filename, line, cudaGetErrorName(error), cudaGetErrorString(error));
    exit(EXIT_FAILURE);
  }  
}

// Substitute recursion for a for loop to avoid stack overflow
__device__ vec3 rayColor(const ray &r, World world, curandState *local_rand_state) {
  ray cur_ray = r;
  vec3 cur_attenuation = vec3(1.0, 1.0, 1.0);
  
  for (i32 i = 0; i < world.ray_max_depth; i++) {
    hit_record rec;
    if ((*(world.collider))->hit(cur_ray, 0.001f, INF, rec)) {
      ray scattered;
      vec3 attenuation;
      if (rec.mat_ptr->scatter(cur_ray, rec, attenuation, scattered, local_rand_state)) {
        cur_attenuation = cur_attenuation * attenuation;
        cur_ray = scattered;
      } else {
        return vec3(0.0, 0.0, 0.0);
      }
    } else {
      vec3 unit_direction = normalize(cur_ray.direction());
      f32 t = 0.5f * (unit_direction.y() + 1.0f);
      vec3 c = (1.0f - t) * world.sky_color1 + t * world.sky_color2;
      return cur_attenuation * c;
    }
  }
  return vec3(0.0, 0.0, 0.0); // exceeded recursion
}

__global__ void rand_init(curandState *rand_state) {
  if (threadIdx.x == 0 && blockIdx.x == 0) {
    curand_init(970, 0, 0, rand_state);
  }
}

__global__ void render_init(i32 width, int height, curandState *rand_state) {
  i32 i = threadIdx.x + blockIdx.x * blockDim.x;
  i32 j = threadIdx.y + blockIdx.y * blockDim.y;
  if ((i >= width) || (j >= height) || i < 0 || j < 0) return;
  
  i32 pixel_index = j * width + i;
  curand_init(970 + pixel_index, 0, 0, &rand_state[pixel_index]);
}

__global__ void rayTrace(vec3 *fb, i32 width, i32 height, World world, curandState *rand_state) {
  i32 i = threadIdx.x + blockIdx.x * blockDim.x;
  i32 j = threadIdx.y + blockIdx.y * blockDim.y;
  if ((i >= width) || (j >= height) || i < 0 || j < 0) return;
  
  i32 pixel_index = j * width + i;
  curandState local_rand_state = rand_state[pixel_index];
  
  vec3 col(0, 0, 0);
  for (i32 s = 0; s < world.pixel_samples; s++) {
    f32 u    = f32(i + curand_uniform(&local_rand_state)) / f32(width);
    f32 v    = f32(j + curand_uniform(&local_rand_state)) / f32(height);
    ray r    = (*world.camera)->get_ray(u, v, &local_rand_state);
    col      = col + rayColor(r, world, &local_rand_state);
  }
  
  rand_state[pixel_index] = local_rand_state;
  col      = col / f32(world.pixel_samples);
  col = vec3(sqrt(col.x()), sqrt(col.y()), sqrt(col.z()));
   
  fb[pixel_index] = col;
}

i32 main() {
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
  const GLubyte* vendor = glGetString(GL_VENDOR);
  
  printf("OpenGL Vendor: %s\n", vendor);
  printf("Rendering a %dx%d image with %d samples per pixel.\n", width, height, pixel_samples);
  nvtxRangePush("rayTrace"); 
  
  //------------------------------------
  // World + Camera 
  //------------------------------------  
  // we need that a random state to be initialized for the world creation 
  curandState *d_rand_state_world;
  checkCudaErrors(cudaMalloc((void **)&d_rand_state_world, 1 * sizeof(curandState)));
  rand_init<<<1, 1>>>(d_rand_state_world);
  checkCudaErrors(cudaGetLastError());
  checkCudaErrors(cudaDeviceSynchronize());

  // make our world of hittables objects and the main camera 
  World world;
  checkCudaErrors(cudaMalloc((void **) &(world.objects), WORLD_SPACE * sizeof(hittable *)));  
  checkCudaErrors(cudaMalloc((void **) &(world.collider), sizeof(hittable *))); 
  checkCudaErrors(cudaMalloc((void **) &(world.camera), sizeof(Camera *))); 
  
  // simple_world<<<1, 1>>>(world.objects, world.collider, world.camera, aspect_ratio);  
  book_cover_world<<<1, 1>>>(world.objects, world.collider, world.camera, aspect_ratio, d_rand_state_world); 
  
  world.sky_color1     = vec3(1, 1, 1);
  world.sky_color2     = vec3(0.5, 0.7, 1.0);  
  
  world.pixel_samples  = pixel_samples;
  world.ray_max_depth  = ray_max_depth;
  
  //------------------------------------
  // Prepare Render Texture and run RayTracing
  //------------------------------------   
  // Allocate our frame buffer
  i32 num_pixels = width * height;
  vec3 *frame_buffer; 
  checkCudaErrors(cudaMallocManaged((void **)&frame_buffer, num_pixels * sizeof(vec3))); 
  
  // Blocks and Threads
  i32 num_threads_x = 8;
  i32 num_threads_y = 8;  
  dim3 blocks(width / num_threads_x + 1, height / num_threads_y + 1);
  dim3 threads(num_threads_x, num_threads_y);
  
  // Allocate random state
  curandState *d_rand_state_trace;
  checkCudaErrors(cudaMalloc((void **)&d_rand_state_trace, num_pixels * sizeof(curandState)));
  render_init<<<blocks, threads>>>(width, height, d_rand_state_trace);  

  // Raytrace
  printf("RayTracing...\n"); 
  clock_t start, stop;
  start = clock(); 
  checkCudaErrors(cudaGetLastError());
  checkCudaErrors(cudaDeviceSynchronize());
  rayTrace<<<blocks, threads>>>(frame_buffer, width, height, world, d_rand_state_trace);
  checkCudaErrors(cudaGetLastError());
  checkCudaErrors(cudaDeviceSynchronize()); 
  stop = clock();
  f64 timer_seconds = ((f64)(stop - start)) / CLOCKS_PER_SEC; 
  printf("Took %f s\n", timer_seconds);

  // Copy Frame Buffer into a Texture
  u8 *texture_data = (u8 *) malloc(width * height * 4);
  memset(texture_data, 0, width * height * 4); 
  for (i32 j = height - 1; j >= 0; j--) {
    for (i32 i = 0; i < width; i++) {
      i32 index = ((j)*width + i) * 4;
      i32 pixel_index = j * width + i;
      texture_data[index]     = (u8)(255.0f * frame_buffer[pixel_index].r());
      texture_data[index + 1] = (u8)(255.0f * frame_buffer[pixel_index].g());
      texture_data[index + 2] = (u8)(255.0f * frame_buffer[pixel_index].b());
      texture_data[index + 3] = 255;
    }
  }

  // Free CUDA Memory
  checkCudaErrors(cudaDeviceSynchronize());
  checkCudaErrors(cudaGetLastError());
  checkCudaErrors(cudaFree(world.camera));
  checkCudaErrors(cudaFree(world.collider));
  checkCudaErrors(cudaFree(world.objects));
  checkCudaErrors(cudaFree(d_rand_state_world));
  checkCudaErrors(cudaFree(d_rand_state_trace));
  checkCudaErrors(cudaFree(frame_buffer));
  cudaDeviceReset(); 
  nvtxRangePop();

  //------------------------------------
  // Render to the window
  // TODO: Make it real time 
  //------------------------------------     
  while (!glfwWindowShouldClose(windowContext.glfw_window)) {
    glClear(GL_COLOR_BUFFER_BIT);
 
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

  // Free Texture and Save Image to memory
  free(texture_data);
  saveRenderTexture(windowContext.renderer.texture, width, height, "raytraced_image.png");

  // Terminate Window Context data structures
  terminateRenderer(&windowContext.renderer);
  terminateWindowGLFW(windowContext.glfw_window);
  
}
