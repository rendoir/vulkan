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
include Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/depend.make

# Include the progress variables for this target.
include Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/progress.make

# Include the compile flags for this target's objects.
include Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/flags.make

../Layer-Samples/data/overlay-vert.spv: ../Layer-Samples/data/overlay.vert
../Layer-Samples/data/overlay-vert.spv: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/bin/glslangValidator
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Generating ../../../Layer-Samples/data/overlay-vert.spv"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/bin/glslangValidator -s -V /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/Layer-Samples/data/overlay.vert
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && mv vert.spv /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/Layer-Samples/data/overlay-vert.spv

../Layer-Samples/data/overlay-frag.spv: ../Layer-Samples/data/overlay.frag
../Layer-Samples/data/overlay-frag.spv: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/bin/glslangValidator
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Generating ../../../Layer-Samples/data/overlay-frag.spv"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/bin/glslangValidator -s -V /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/Layer-Samples/data/overlay.frag
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && mv frag.spv /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/Layer-Samples/data/overlay-frag.spv

Layer-Samples/Overlay/overlay.json:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --blue --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Generating overlay.json"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && /usr/bin/cmake -E copy_if_different /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/Layer-Samples/Overlay/linux/overlay.json overlay.json

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/flags.make
Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o: ../Layer-Samples/Overlay/overlay.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o -c /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/Layer-Samples/Overlay/overlay.cpp

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/VKLayer_overlay.dir/overlay.cpp.i"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/Layer-Samples/Overlay/overlay.cpp > CMakeFiles/VKLayer_overlay.dir/overlay.cpp.i

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/VKLayer_overlay.dir/overlay.cpp.s"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/Layer-Samples/Overlay/overlay.cpp -o CMakeFiles/VKLayer_overlay.dir/overlay.cpp.s

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o.requires:

.PHONY : Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o.requires

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o.provides: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o.requires
	$(MAKE) -f Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/build.make Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o.provides.build
.PHONY : Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o.provides

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o.provides.build: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o


Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/flags.make
Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o -c /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.i"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp > CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.i

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.s"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp -o CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.s

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o.requires:

.PHONY : Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o.requires

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o.provides: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o.requires
	$(MAKE) -f Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/build.make Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o.provides.build
.PHONY : Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o.provides

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o.provides.build: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o


# Object files for target VKLayer_overlay
VKLayer_overlay_OBJECTS = \
"CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o" \
"CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o"

# External object files for target VKLayer_overlay
VKLayer_overlay_EXTERNAL_OBJECTS =

Layer-Samples/Overlay/libVKLayer_overlay.so: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o
Layer-Samples/Overlay/libVKLayer_overlay.so: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o
Layer-Samples/Overlay/libVKLayer_overlay.so: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/build.make
Layer-Samples/Overlay/libVKLayer_overlay.so: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/x86_64/lib/libVkLayer_utils.a
Layer-Samples/Overlay/libVKLayer_overlay.so: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX shared library libVKLayer_overlay.so"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/VKLayer_overlay.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/build: Layer-Samples/Overlay/libVKLayer_overlay.so

.PHONY : Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/build

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/requires: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/overlay.cpp.o.requires
Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/requires: Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/layers/vk_layer_table.cpp.o.requires

.PHONY : Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/requires

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/clean:
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay && $(CMAKE_COMMAND) -P CMakeFiles/VKLayer_overlay.dir/cmake_clean.cmake
.PHONY : Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/clean

Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/depend: ../Layer-Samples/data/overlay-vert.spv
Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/depend: ../Layer-Samples/data/overlay-frag.spv
Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/depend: Layer-Samples/Overlay/overlay.json
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/Layer-Samples/Overlay /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : Layer-Samples/Overlay/CMakeFiles/VKLayer_overlay.dir/depend

