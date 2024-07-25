
#include "windowContext.h"

// Create GLFW window to display the rendering
GLFWwindow* initWindowGLFW(u32 width, u32 height, const char* title) { 
  if(!glfwInit()) {
    ERROR_EXIT("Failed to initialize GLFW\n");
    return NULL;
  }
  
  // Hint for the correct version
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  // Create Window 
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

  // Setup callbacks
  glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
  glfwSetKeyCallback(window, keyCallback);  

  // Setup initial viewport size
  i32 framebufferWidth, framebufferHeight;
  glfwGetFramebufferSize(window, &framebufferWidth, &framebufferHeight); 
  glViewport(0, 0, framebufferWidth, framebufferHeight); 
  
  return window;
}

void terminateWindowGLFW(GLFWwindow* window) {
  glfwDestroyWindow(window);
  glfwTerminate();
}
