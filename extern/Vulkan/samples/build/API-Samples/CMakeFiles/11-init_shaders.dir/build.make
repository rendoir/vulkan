# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.10

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build

# Include any dependencies generated for this target.
include API-Samples/CMakeFiles/11-init_shaders.dir/depend.make

# Include the progress variables for this target.
include API-Samples/CMakeFiles/11-init_shaders.dir/progress.make

# Include the compile flags for this target's objects.
include API-Samples/CMakeFiles/11-init_shaders.dir/flags.make

API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o: API-Samples/CMakeFiles/11-init_shaders.dir/flags.make
API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o: ../API-Samples/11-init_shaders/11-init_shaders.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o -c /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/11-init_shaders/11-init_shaders.cpp

API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.i"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/11-init_shaders/11-init_shaders.cpp > CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.i

API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.s"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/11-init_shaders/11-init_shaders.cpp -o CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.s

API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o.requires:

.PHONY : API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o.requires

API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o.provides: API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o.requires
	$(MAKE) -f API-Samples/CMakeFiles/11-init_shaders.dir/build.make API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o.provides.build
.PHONY : API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o.provides

API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o.provides.build: API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o


# Object files for target 11-init_shaders
11__init_shaders_OBJECTS = \
"CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o"

# External object files for target 11-init_shaders
11__init_shaders_EXTERNAL_OBJECTS =

API-Samples/11-init_shaders: API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o
API-Samples/11-init_shaders: API-Samples/CMakeFiles/11-init_shaders.dir/build.make
API-Samples/11-init_shaders: /usr/lib/x86_64-linux-gnu/libxcb.so
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/libvulkan.so
API-Samples/11-init_shaders: API-Samples/utils/libvsamputils.a
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libglslang.a
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libOGLCompiler.a
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libOSDependent.a
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libHLSL.a
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libSPIRV.a
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libSPVRemapper.a
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/spirv-tools/libSPIRV-Tools-opt.a
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/spirv-tools/libSPIRV-Tools.a
API-Samples/11-init_shaders: /usr/lib/x86_64-linux-gnu/libxcb.so
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/libvulkan.so
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/spirv-tools/libSPIRV-Tools-opt.a
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/spirv-tools/libSPIRV-Tools.a
API-Samples/11-init_shaders: /usr/lib/x86_64-linux-gnu/libxcb.so
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/libvulkan.so
API-Samples/11-init_shaders: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/libvulkan.so
API-Samples/11-init_shaders: API-Samples/CMakeFiles/11-init_shaders.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable 11-init_shaders"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/11-init_shaders.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
API-Samples/CMakeFiles/11-init_shaders.dir/build: API-Samples/11-init_shaders

.PHONY : API-Samples/CMakeFiles/11-init_shaders.dir/build

API-Samples/CMakeFiles/11-init_shaders.dir/requires: API-Samples/CMakeFiles/11-init_shaders.dir/11-init_shaders/11-init_shaders.cpp.o.requires

.PHONY : API-Samples/CMakeFiles/11-init_shaders.dir/requires

API-Samples/CMakeFiles/11-init_shaders.dir/clean:
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples && $(CMAKE_COMMAND) -P CMakeFiles/11-init_shaders.dir/cmake_clean.cmake
.PHONY : API-Samples/CMakeFiles/11-init_shaders.dir/clean

API-Samples/CMakeFiles/11-init_shaders.dir/depend:
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/CMakeFiles/11-init_shaders.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : API-Samples/CMakeFiles/11-init_shaders.dir/depend

