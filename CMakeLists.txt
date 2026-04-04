cmake_minimum_required(VERSION 3.23)
project(roar)
include(FetchContent)

# ----- Third Party -----
find_package(Vulkan REQUIRED)
find_program(GLSLC glslc REQUIRED)

# ----- Dependencies -----

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Skip GLFW examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Skip GLFW tests" FORCE)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Skip GLFW documentation" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Skip GLFW install" FORCE)
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG 3.4
)

set(GLM_TEST_ENABLE OFF CACHE BOOL "Skip GLM install" FORCE)
FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG 1.0.3
)

set(ENTT_BUILD_TESTING OFF CACHE BOOL "Skip ENTT install" FORCE)
FetchContent_Declare(
    entt
    GIT_REPOSITORY https://github.com/skypjack/entt.git
    GIT_TAG v3.16.0
)

set(JSON_BuildTests OFF CACHE BOOL "Skip JSON tests" FORCE)
set(JSON_Install OFF CACHE BOOL "Skip JSON install" FORCE)
FetchContent_Declare(
    json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.12.0
)

set(TINYGLTF_BUILD_LOADER_EXAMPLE OFF CACHE BOOL "Skip TinyGLTF example" FORCE)
set(TINYGLTF_BUILD_GL_EXAMPLES OFF CACHE BOOL "Skip TinyGLTF example" FORCE)
FetchContent_Declare(
    tinygltf
    GIT_REPOSITORY https://github.com/syoyo/tinygltf.git
    GIT_TAG v3.0.0
)

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)

FetchContent_MakeAvailable(
    glfw
    glm
    entt
    json
    tinygltf
    stb
)


# ----- Source -----

# Shaders
set(GLSL_SHADER_PATH ${PROJECT_SOURCE_DIR}/shaders)
set(SPIRV_SHADER_PATH ${PROJECT_BINARY_DIR}/.spirv)
file(MAKE_DIRECTORY ${SPIRV_SHADER_PATH})
file(GLOB_RECURSE GLSL_SHADERS ${GLSL_SHADER_PATH}/*.vert ${GLSL_SHADER_PATH}/*.frag)
foreach(FILE ${GLSL_SHADERS})
	file(RELATIVE_PATH FILENAME ${GLSL_SHADER_PATH} ${FILE})
	set(SPIRV_FILE ${SPIRV_SHADER_PATH}/${FILENAME}.spv)
	add_custom_command(
		OUTPUT ${SPIRV_FILE}
		COMMAND ${GLSLC} ${FILE} -o ${SPIRV_FILE}
		COMMENT "Compiling shader: ${FILE}"
		DEPENDS ${FILE}
		VERBATIM
	)
	list(APPEND SPIRV_SHADERS ${SPIRV_FILE})
endforeach()
add_custom_target(shaders DEPENDS ${SPIRV_SHADERS})

# Code
set(SOURCE_PATH ${PROJECT_SOURCE_DIR}/source)
file(GLOB_RECURSE SOURCE_FILES ${SOURCE_PATH}/*.cpp ${SOURCE_PATH}/*.c ${SOURCE_PATH}/*.hpp ${SOURCE_PATH}/*.h)

# Project
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_include_directories(${PROJECT_NAME} PUBLIC ${SOURCE_PATH})
target_include_directories(${PROJECT_NAME} PUBLIC ${Vulkan_INCLUDE_DIRS})
target_include_directories(${PROJECT_NAME} PRIVATE
    ${stb_SOURCE_DIR}
    ${tinygltf_SOURCE_DIR}
)

target_precompile_headers(${PROJECT_NAME} PUBLIC ${SOURCE_PATH}/PCH.hpp)
target_compile_definitions(${PROJECT_NAME} PUBLIC SPIRV_SHADER_PATH="${SPIRV_SHADER_PATH}")

target_link_libraries(${PROJECT_NAME} PRIVATE
    Vulkan::Vulkan
    glfw
    glm
    EnTT::EnTT
    nlohmann_json::nlohmann_json
)

add_dependencies(${PROJECT_NAME} shaders)

# Compiler options
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 23)

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /wd4267 /wd4244)
elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra)
endif()
