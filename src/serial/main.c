
#include <stdio.h>

#define ARENA_IMPLEMENTATION
#include "../../ext/arena_allocator.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../ext/stb_image_write.h"

#include "utils/types.h"
#include "utils/utils.h"


#include "../../ext/glad/include/glad.h"
#include "../../ext/glfw/include/GLFW/glfw3.h"
GLFWwindow* initGLFW(int width, int height, const char* title) {
  if(!glfwInit()) {
    ERROR_EXIT("Failed to initialize GLFW\n");
    return NULL;
  }

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  GLFWwindow* window = glfwCreateWindow(width, height, title, NULL, NULL); 
  if(!window) {
    ERROR_EXIT("Failed to create GLFW window\n");
    glfwTerminate();
    return NULL;
  }

  glfwMakeContextCurrent(window);

  // Initialize GLAD
  if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    ERROR_EXIT("Failed to initialize GLAD\n");
    glfwDestroyWindow(window);
    glfwTerminate();
    return NULL;
  }

  return window;
}



int main(){ 

  const int width   = 256;
  const int height  = 256;
  const char* title = "Luminara";

  GLFWwindow* window = initGLFW(width, height, title);
    
  while (!glfwWindowShouldClose(window)) {
    glfwPollEvents();
    glClearColor(0.0, 0.3, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
  }

  glfwDestroyWindow(window);
  glfwTerminate();
}



// static Arena default_arena = {0};
//
// int main(){
//   u32 image_width  = 2*256;
//   u32 image_height = 2*256;
//   u32 image_size   = image_width * image_height * 3;
//   
//   u8* image_data = arena_alloc(&default_arena, image_size * sizeof(u8)); 
//   if(image_data == NULL){
//     fprintf(stderr, "Error allocating image data\n");
//     return 0;
//   } 
//   
//   for(int y = 0; y < image_height; y++){
//     printf("\rScanlines remaining: %d ", (image_height - y));
//     fflush(stdout);
//     
//     for(int x = 0; x < image_width; x++){    
//       // Compute color floats
//       f32 float_r = (f32)x / (f32)image_width;
//       f32 float_g = (f32)y / (f32)image_height;
//       f32 float_b = 0.0f;
//
//       // Convert floats to bytes
//       u8* color = arena_alloc(&default_arena, 3 * sizeof(u8));
//       u8 r = (u8)(255.0f * float_r);
//       u8 g = (u8)(255.0f * float_g);
//       u8 b = (u8)(255.0f * float_b);
//
//       // Set pixel
//       u32 pixel_index = (y * image_width + x) * 3; 
//       image_data[pixel_index]   = r;
//       image_data[pixel_index+1] = g;
//       image_data[pixel_index+2] = b;
//     }
//   }
//
//   // Write image
//   if(!stbi_write_png("render.png", image_width, image_height, 3, image_data, image_width * 3)) {
//     fprintf(stderr, "Error writing image to file\n");
//     free(image_data);
//     return 0;
//   }
//   printf("\nImage rendered successfully!\n");  
// }
