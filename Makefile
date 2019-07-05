INC_DIR = -I./include -isystem ./extern -isystem ./extern/GLFW/include -isystem ./extern/Vulkan/include
LIB_DIR = -L./extern/GLFW/lib -L./extern/Vulkan/lib
LIB = -lglfw3 -ldl -lX11 -lpthread -lXxf86vm -lXrandr -lXi -lXinerama -lXcursor -lvulkan

CXX = g++
CXX_STD = -std=c++17
CXX_WARNINGS = 
CXX_FLAGS = $(CXX_STD) $(CXX_WARNINGS) $(INC_DIR) $(LIB_DIR)

SHADERC = ./extern/Vulkan/bin/glslangValidator

# Final binary
BIN = roar
# Put all auto generated stuff to this build dir
BUILD_DIR = ./build

# List of all .cpp source files
CPP = $(wildcard source/*.cpp)
# List of all glsl shader files
GLSL = $(wildcard shaders/*)

# All .o files go to build dir
OBJ = $(CPP:%.cpp=$(BUILD_DIR)/%.o)
# The compiler will create these .d files containing dependencies
DEP = $(OBJ:%.o=%.d)
# All .spv files
SPV = $(GLSL:%=$(BUILD_DIR)/%.spv)

# Default target named after the binary.
$(BIN) : $(BUILD_DIR)/$(BIN)

# Actual target of the binary - depends on all .o files
$(BUILD_DIR)/$(BIN) : $(OBJ)
#	Just link all the object files
	$(CXX) $(CXX_FLAGS) $(OBJ) -o $@ $(LIB)

# Actual target of the binary - depends on all .spv files
$(BUILD_DIR)/$(BIN) : $(SPV)

# Include all .d files
-include $(DEP)

# Build target for every single object file
# The potential dependency on header files is covered
# by calling `-include $(DEP)`
$(BUILD_DIR)/%.o : %.cpp
	mkdir -p $(@D)
# 	The -MMD flags additionaly creates a .d file with
# 	the same name as the .o file
	$(CXX) $(CXX_FLAGS) -MMD -c $< -o $@ $(LIB)

# Build target for every single spirv file
$(BUILD_DIR)/%.vs.spv : %.vs
	mkdir -p $(@D)
	$(SHADERC) -V $< -S vert -o $@

# Build target for every single spirv file
$(BUILD_DIR)/%.fs.spv : %.fs
	mkdir -p $(@D)
	$(SHADERC) -V $< -S frag -o $@

.PHONY : clean
clean :
#	 This should remove all generated files
	-rm -rf $(BUILD_DIR)/$(BIN) $(OBJ) $(DEP) $(SPV)
