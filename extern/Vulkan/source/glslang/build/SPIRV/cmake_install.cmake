# Install script for directory: /home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/SPIRV/libSPVRemapper.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/build/SPIRV/libSPIRV.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SPIRV" TYPE FILE FILES
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/bitutils.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/spirv.hpp"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/GLSL.std.450.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/GLSL.ext.EXT.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/GLSL.ext.KHR.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/GlslangToSpv.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/hex_float.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/Logger.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/SpvBuilder.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/spvIR.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/doc.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/disassemble.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/GLSL.ext.AMD.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/GLSL.ext.NV.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/SPVRemapper.h"
    "/home/rendoir/Downloads/vulkansdk-linux-x86_64-1.1.82.1/1.1.82.1/source/glslang/SPIRV/doc.h"
    )
endif()

