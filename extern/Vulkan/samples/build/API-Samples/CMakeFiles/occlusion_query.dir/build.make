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
include API-Samples/CMakeFiles/occlusion_query.dir/depend.make

# Include the progress variables for this target.
include API-Samples/CMakeFiles/occlusion_query.dir/progress.make

# Include the compile flags for this target's objects.
include API-Samples/CMakeFiles/occlusion_query.dir/flags.make

API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o: API-Samples/CMakeFiles/occlusion_query.dir/flags.make
API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o: ../API-Samples/occlusion_query/occlusion_query.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o -c /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/occlusion_query/occlusion_query.cpp

API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.i"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/occlusion_query/occlusion_query.cpp > CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.i

API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.s"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/occlusion_query/occlusion_query.cpp -o CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.s

API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o.requires:

.PHONY : API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o.requires

API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o.provides: API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o.requires
	$(MAKE) -f API-Samples/CMakeFiles/occlusion_query.dir/build.make API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o.provides.build
.PHONY : API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o.provides

API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o.provides.build: API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o


# Object files for target occlusion_query
occlusion_query_OBJECTS = \
"CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o"

# External object files for target occlusion_query
occlusion_query_EXTERNAL_OBJECTS =

API-Samples/occlusion_query: API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o
API-Samples/occlusion_query: API-Samples/CMakeFiles/occlusion_query.dir/build.make
API-Samples/occlusion_query: /usr/lib/x86_64-linux-gnu/libxcb.so
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/libvulkan.so
API-Samples/occlusion_query: API-Samples/utils/libvsamputils.a
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libglslang.a
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libOGLCompiler.a
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libOSDependent.a
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libHLSL.a
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libSPIRV.a
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/glslang/libSPVRemapper.a
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/spirv-tools/libSPIRV-Tools-opt.a
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/spirv-tools/libSPIRV-Tools.a
API-Samples/occlusion_query: /usr/lib/x86_64-linux-gnu/libxcb.so
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/libvulkan.so
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/spirv-tools/libSPIRV-Tools-opt.a
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/spirv-tools/libSPIRV-Tools.a
API-Samples/occlusion_query: /usr/lib/x86_64-linux-gnu/libxcb.so
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/libvulkan.so
API-Samples/occlusion_query: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/libvulkan.so
API-Samples/occlusion_query: API-Samples/CMakeFiles/occlusion_query.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable occlusion_query"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/occlusion_query.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
API-Samples/CMakeFiles/occlusion_query.dir/build: API-Samples/occlusion_query

.PHONY : API-Samples/CMakeFiles/occlusion_query.dir/build

API-Samples/CMakeFiles/occlusion_query.dir/requires: API-Samples/CMakeFiles/occlusion_query.dir/occlusion_query/occlusion_query.cpp.o.requires

.PHONY : API-Samples/CMakeFiles/occlusion_query.dir/requires

API-Samples/CMakeFiles/occlusion_query.dir/clean:
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples && $(CMAKE_COMMAND) -P CMakeFiles/occlusion_query.dir/cmake_clean.cmake
.PHONY : API-Samples/CMakeFiles/occlusion_query.dir/clean

API-Samples/CMakeFiles/occlusion_query.dir/depend:
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/CMakeFiles/occlusion_query.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : API-Samples/CMakeFiles/occlusion_query.dir/depend

