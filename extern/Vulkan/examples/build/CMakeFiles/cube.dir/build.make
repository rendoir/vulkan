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
CMAKE_SOURCE_DIR = /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build

# Include any dependencies generated for this target.
include CMakeFiles/cube.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/cube.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/cube.dir/flags.make

cube.vert.inc: ../cube.vert
cube.vert.inc: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/bin/glslangValidator
cube.vert.inc: ../cube.vert
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Compiling cube demo vertex shader"
	/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/bin/glslangValidator -V -x -o /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build/cube.vert.inc /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/cube.vert

cube.frag.inc: ../cube.frag
cube.frag.inc: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/bin/glslangValidator
cube.frag.inc: ../cube.frag
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Compiling cube demo fragment shader"
	/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/bin/glslangValidator -V -x -o /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build/cube.frag.inc /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/cube.frag

CMakeFiles/cube.dir/cube.c.o: CMakeFiles/cube.dir/flags.make
CMakeFiles/cube.dir/cube.c.o: ../cube.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building C object CMakeFiles/cube.dir/cube.c.o"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/cube.dir/cube.c.o   -c /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/cube.c

CMakeFiles/cube.dir/cube.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/cube.dir/cube.c.i"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/cube.c > CMakeFiles/cube.dir/cube.c.i

CMakeFiles/cube.dir/cube.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/cube.dir/cube.c.s"
	/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/cube.c -o CMakeFiles/cube.dir/cube.c.s

CMakeFiles/cube.dir/cube.c.o.requires:

.PHONY : CMakeFiles/cube.dir/cube.c.o.requires

CMakeFiles/cube.dir/cube.c.o.provides: CMakeFiles/cube.dir/cube.c.o.requires
	$(MAKE) -f CMakeFiles/cube.dir/build.make CMakeFiles/cube.dir/cube.c.o.provides.build
.PHONY : CMakeFiles/cube.dir/cube.c.o.provides

CMakeFiles/cube.dir/cube.c.o.provides.build: CMakeFiles/cube.dir/cube.c.o


# Object files for target cube
cube_OBJECTS = \
"CMakeFiles/cube.dir/cube.c.o"

# External object files for target cube
cube_EXTERNAL_OBJECTS =

cube: CMakeFiles/cube.dir/cube.c.o
cube: CMakeFiles/cube.dir/build.make
cube: /usr/lib/x86_64-linux-gnu/libxcb.so
cube: CMakeFiles/cube.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking C executable cube"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/cube.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/cube.dir/build: cube

.PHONY : CMakeFiles/cube.dir/build

CMakeFiles/cube.dir/requires: CMakeFiles/cube.dir/cube.c.o.requires

.PHONY : CMakeFiles/cube.dir/requires

CMakeFiles/cube.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/cube.dir/cmake_clean.cmake
.PHONY : CMakeFiles/cube.dir/clean

CMakeFiles/cube.dir/depend: cube.vert.inc
CMakeFiles/cube.dir/depend: cube.frag.inc
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/examples/build/CMakeFiles/cube.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/cube.dir/depend
