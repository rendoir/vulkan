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
include API-Samples/utils/CMakeFiles/vsamputils.dir/depend.make

# Include the progress variables for this target.
include API-Samples/utils/CMakeFiles/vsamputils.dir/progress.make

# Include the compile flags for this target's objects.
include API-Samples/utils/CMakeFiles/vsamputils.dir/flags.make

API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o: API-Samples/utils/CMakeFiles/vsamputils.dir/flags.make
API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o: ../API-Samples/utils/util.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vsamputils.dir/util.cpp.o -c /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/utils/util.cpp

API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vsamputils.dir/util.cpp.i"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/utils/util.cpp > CMakeFiles/vsamputils.dir/util.cpp.i

API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vsamputils.dir/util.cpp.s"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/utils/util.cpp -o CMakeFiles/vsamputils.dir/util.cpp.s

API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o.requires:

.PHONY : API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o.requires

API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o.provides: API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o.requires
	$(MAKE) -f API-Samples/utils/CMakeFiles/vsamputils.dir/build.make API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o.provides.build
.PHONY : API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o.provides

API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o.provides.build: API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o


API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o: API-Samples/utils/CMakeFiles/vsamputils.dir/flags.make
API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o: ../API-Samples/utils/util_init.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils && /usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/vsamputils.dir/util_init.cpp.o -c /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/utils/util_init.cpp

API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/vsamputils.dir/util_init.cpp.i"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/utils/util_init.cpp > CMakeFiles/vsamputils.dir/util_init.cpp.i

API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/vsamputils.dir/util_init.cpp.s"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils && /usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/utils/util_init.cpp -o CMakeFiles/vsamputils.dir/util_init.cpp.s

API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o.requires:

.PHONY : API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o.requires

API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o.provides: API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o.requires
	$(MAKE) -f API-Samples/utils/CMakeFiles/vsamputils.dir/build.make API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o.provides.build
.PHONY : API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o.provides

API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o.provides.build: API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o


# Object files for target vsamputils
vsamputils_OBJECTS = \
"CMakeFiles/vsamputils.dir/util.cpp.o" \
"CMakeFiles/vsamputils.dir/util_init.cpp.o"

# External object files for target vsamputils
vsamputils_EXTERNAL_OBJECTS =

API-Samples/utils/libvsamputils.a: API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o
API-Samples/utils/libvsamputils.a: API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o
API-Samples/utils/libvsamputils.a: API-Samples/utils/CMakeFiles/vsamputils.dir/build.make
API-Samples/utils/libvsamputils.a: API-Samples/utils/CMakeFiles/vsamputils.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libvsamputils.a"
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils && $(CMAKE_COMMAND) -P CMakeFiles/vsamputils.dir/cmake_clean_target.cmake
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/vsamputils.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
API-Samples/utils/CMakeFiles/vsamputils.dir/build: API-Samples/utils/libvsamputils.a

.PHONY : API-Samples/utils/CMakeFiles/vsamputils.dir/build

API-Samples/utils/CMakeFiles/vsamputils.dir/requires: API-Samples/utils/CMakeFiles/vsamputils.dir/util.cpp.o.requires
API-Samples/utils/CMakeFiles/vsamputils.dir/requires: API-Samples/utils/CMakeFiles/vsamputils.dir/util_init.cpp.o.requires

.PHONY : API-Samples/utils/CMakeFiles/vsamputils.dir/requires

API-Samples/utils/CMakeFiles/vsamputils.dir/clean:
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils && $(CMAKE_COMMAND) -P CMakeFiles/vsamputils.dir/cmake_clean.cmake
.PHONY : API-Samples/utils/CMakeFiles/vsamputils.dir/clean

API-Samples/utils/CMakeFiles/vsamputils.dir/depend:
	cd /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/API-Samples/utils /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/samples/build/API-Samples/utils/CMakeFiles/vsamputils.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : API-Samples/utils/CMakeFiles/vsamputils.dir/depend
