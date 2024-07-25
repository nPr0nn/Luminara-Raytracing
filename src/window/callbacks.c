
#include "windowContext.h"

void framebufferSizeCallback(GLFWwindow* window, i32 width, i32 height) {
  glViewport(0, 0, width, height);
}

void keyCallback(GLFWwindow* window, i32 key, i32 scancode, i32 action, i32 mods) {
  // Close window
  if( (key == GLFW_KEY_Q || key == GLFW_KEY_ESCAPE) && action == GLFW_PRESS) {
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  } 
}


