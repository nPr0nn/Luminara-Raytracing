
# GLFW Directories
GLFW_DIR := ext/glfw
GLFW_BUILD_DIR := $(GLFW_DIR)/build
GLAD_DIR := ext/glad/src

# Targets
.PHONY: all glfw render clean

# Source Files - Window
C_FILES   = src/window/glfw_window.c \
						src/window/callbacks.c \
						src/window/shaders.c \
						src/window/renderer.c \
						$(GLAD_DIR)/glad.c

# Source Files C++
CPP_FILES  = src/main.cpp \

# Source Files CUDA
CUDA_FILES = src/main.cu \

# Object Files
C_OBJS    = $(C_FILES:.c=.o)
CUDA_OBJS = $(CUDA_FILES:.cu=.o)

all: glfw render render_cuda

glfw:
	@echo "Building GLFW..."
	@mkdir -p $(GLFW_BUILD_DIR)
	@cd $(GLFW_BUILD_DIR) && cmake .. && $(MAKE)

render:
	@echo "Building render..."
	@g++ $(C_FILES) $(CPP_FILES) -o main -L$(GLFW_BUILD_DIR)/src -lglfw3 -lm 
	@echo "Render built successfully. Running..." 
	@./main

%.o: %.c
	@gcc -I$(GLFW_DIR)/include -I$(GLAD_DIR) -c $< -o $@
%.o: %.cu
	@nvcc -I$(GLFW_DIR)/include -I$(GLAD_DIR) -c $< -o $@	
render_cuda: $(C_OBJS) $(CUDA_OBJS)
	@echo "Building render..."
	@nvcc $(C_OBJS) $(CUDA_OBJS) -g -G -o main -lnvToolsExt -L$(GLFW_BUILD_DIR)/src -lglfw3 -lm
	@echo "Render built successfully. Running..." 
	@./main

profile_render_cuda:
	@echo "Building render..."
	@nvcc $(C_OBJS) $(CUDA_OBJS) -g -G -o main -lnvToolsExt -L$(GLFW_BUILD_DIR)/src -lglfw3 -lm	
	nsys profile --trace=nvtx,cuda,opengl,osrt --output=profile_report ./main
	nsys-ui profile_report.nsys-rep

clean:
	@echo "Cleaning up..."
	@rm -rf $(GLFW_BUILD_DIR)
	@rm -f main
	@echo "Cleanup complete."

