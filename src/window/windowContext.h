#ifndef WINDOW_CONTEXT_H
#define WINDOW_CONTEXT_H

// external libs
#include "../../ext/stb_image_write.h"
#include "../../ext/glad/include/glad.h"
#include "../../ext/glfw/include/GLFW/glfw3.h"

// Utils
#include "../utils/utils.h"

#ifdef __cplusplus
extern "C" {
#endif

// Renderer Struct 
typedef struct {
  // Textures
  GLuint texture;
  GLuint shaderProgram; 
  
  // Array and Buffers
  GLuint VAO, VBO, EBO;
} Renderer;

// Window Struct
typedef struct {
  GLFWwindow* glfw_window;
  Renderer    renderer;
} WindowContext;

  // Init functions
  GLFWwindow* initWindowGLFW(u32 width, u32 height, const char* title);
  bool initRenderer(Renderer* renderer, u32 width, u32 height);

  // Texture Rendering
  GLuint compileShader(GLenum type, const char* source);
  GLuint createShaderProgram();
  GLuint createRenderTexture(i32 width, i32 height);
  void   saveRenderTexture(GLuint texture, u32 width, u32 height, const char* filename);

  // Callbacks
  void framebufferSizeCallback(GLFWwindow* window, i32 width, i32 height); 
  void keyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods);

  // Terminate Functions 
  void terminateWindowGLFW(GLFWwindow* window);
  void terminateRenderer(Renderer* renderer);

#ifdef __cplusplus
}
#endif

#endif // ! Window_H
