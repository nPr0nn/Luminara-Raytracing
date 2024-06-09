
glfw:
	cd ext/glfw && mkdir build && cd build && cmake .. && make

render:
	gcc src/serial/main.c ext/glad/src/glad.c -o main -Lext/glfw/build/src -lglfw3 -lm  
	./main
	# feh render.png
	
