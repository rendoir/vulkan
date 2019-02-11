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
CMAKE_SOURCE_DIR = /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build

# Include any dependencies generated for this target.
include External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/depend.make

# Include the progress variables for this target.
include External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/progress.make

# Include the compile flags for this target's objects.
include External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/flags.make

External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o: External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/flags.make
External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o: ../External/spirv-tools/tools/opt/opt.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/External/spirv-tools/tools && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/spirv-opt.dir/opt/opt.cpp.o -c /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/External/spirv-tools/tools/opt/opt.cpp

External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/spirv-opt.dir/opt/opt.cpp.i"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/External/spirv-tools/tools && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/External/spirv-tools/tools/opt/opt.cpp > CMakeFiles/spirv-opt.dir/opt/opt.cpp.i

External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/spirv-opt.dir/opt/opt.cpp.s"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/External/spirv-tools/tools && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/External/spirv-tools/tools/opt/opt.cpp -o CMakeFiles/spirv-opt.dir/opt/opt.cpp.s

External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o.requires:

.PHONY : External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o.requires

External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o.provides: External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o.requires
	$(MAKE) -f External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/build.make External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o.provides.build
.PHONY : External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o.provides

External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o.provides.build: External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o


# Object files for target spirv-opt
spirv__opt_OBJECTS = \
"CMakeFiles/spirv-opt.dir/opt/opt.cpp.o"

# External object files for target spirv-opt
spirv__opt_EXTERNAL_OBJECTS =

External/spirv-tools/tools/spirv-opt: External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o
External/spirv-tools/tools/spirv-opt: External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/build.make
External/spirv-tools/tools/spirv-opt: External/spirv-tools/source/opt/libSPIRV-Tools-opt.a
External/spirv-tools/tools/spirv-opt: External/spirv-tools/source/libSPIRV-Tools.a
External/spirv-tools/tools/spirv-opt: External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable spirv-opt"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/External/spirv-tools/tools && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/spirv-opt.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/build: External/spirv-tools/tools/spirv-opt

.PHONY : External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/build

External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/requires: External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/opt/opt.cpp.o.requires

.PHONY : External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/requires

External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/clean:
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/External/spirv-tools/tools && $(CMAKE_COMMAND) -P CMakeFiles/spirv-opt.dir/cmake_clean.cmake
.PHONY : External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/clean

External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/depend:
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/External/spirv-tools/tools /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/External/spirv-tools/tools /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : External/spirv-tools/tools/CMakeFiles/spirv-opt.dir/depend
