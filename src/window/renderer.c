
#include "windowContext.h"

//-------------------------------------------------------------
// Render Texture
GLuint createRenderTexture(i32 width, i32 height) {
  GLuint texture;
  glGenTextures(1, &texture);
  glBindTexture(GL_TEXTURE_2D, texture);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); 
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  return texture;
}

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../../ext/stb_image_write.h"
void saveRenderTexture(GLuint texture, u32 width, u32 height, const char* filename) {
  glBindTexture(GL_TEXTURE_2D, texture);
  u8* data = (u8*) malloc(width * height * 4);
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

  // Flip vertically because OpenGL is upside down
  u8* flipped_data = (u8*) malloc(width * height * 4);
  for(int y = 0; y < height; y++) {
    memcpy(flipped_data + (height - 1 - y) * width * 4, data + y * width * 4, width * 4);
  }
  
  if(stbi_write_png(filename, width, height, 4, flipped_data, width * 4)) printf("Saved texture to %s\n", filename);
  else fprintf(stderr, "Failed to save texture to %s\n", filename); 
  
  free(data);
  free(flipped_data);
}

//-------------------------------------------------------------
// Renderer
bool initRenderer(Renderer* renderer, u32 width, u32 height) {
  // Create render texture vertex array
  f32 vertices[] = {
      // positions          // texture coords
      -1.0f,  1.0f, 0.0f,  0.0f, 1.0f,
      -1.0f, -1.0f, 0.0f,  0.0f, 0.0f,
       1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
       1.0f,  1.0f, 0.0f,  1.0f, 1.0f
  };
 
  // Create render texture index array
  u32 indices[] = {
      0, 1, 2,
      2, 3, 0
  }; 
 
  // Create render texture shader
  renderer->shaderProgram = createShaderProgram();

  // Fill buffers 
  glGenVertexArrays(1, &renderer->VAO);
  glGenBuffers(1, &renderer->VBO);
  glGenBuffers(1, &renderer->EBO);

  glBindVertexArray(renderer->VAO);

  glBindBuffer(GL_ARRAY_BUFFER, renderer->VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer->EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(f32)));
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  renderer->texture = createRenderTexture(width, height); 
  return true;
}

void terminateRenderer(Renderer* renderer) {
  glDeleteVertexArrays(1, &renderer->VAO);
  glDeleteBuffers(1, &renderer->VBO);
  glDeleteBuffers(1, &renderer->EBO);
  glDeleteTextures(1, &renderer->texture);
  glDeleteProgram(renderer->shaderProgram);  
}
