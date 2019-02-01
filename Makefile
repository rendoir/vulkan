build:
	g++ -std=c++17 -I./include -I./extern/GLFW/include -I./extern/Vulkan/x86_64/include -L./extern/GLFW/lib -L./extern/Vulkan/x86_64/lib source/*.cpp -lglfw3 -ldl -lX11 -lpthread -lXxf86vm -lXrandr -lXi -lXinerama -lXcursor -lvulkan -o roar 

shader:
	./extern/Vulkan/x86_64/bin/glslangValidator -V shaders/triangle.vs -S vert -o shaders/triangle.vs.spv
	./extern/Vulkan/x86_64/bin/glslangValidator -V shaders/triangle.fs -S frag -o shaders/triangle.fs.spv

clean:
	rm -rf roar
