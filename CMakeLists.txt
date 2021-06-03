cmake_minimum_required(VERSION 3.20)
project(roar)

# ----- Third Party -----
set(EXTERNAL_PATH ${PROJECT_SOURCE_DIR}/external)

# Vulkan-Headers
add_subdirectory(${EXTERNAL_PATH}/Vulkan-Headers)

# Vulkan-Loader
add_subdirectory(${EXTERNAL_PATH}/Vulkan-Loader)

# SPIRV-Headers
set(SPIRV_HEADERS_SKIP_EXAMPLES ON CACHE BOOL "Skip SPIRV-Headers examples" FORCE)
add_subdirectory(${EXTERNAL_PATH}/SPIRV-Headers)

# SPIRV-Tools
set(SPIRV_SKIP_TESTS ON CACHE BOOL "Skip SPIRV-Tools tests" FORCE)
add_subdirectory(${EXTERNAL_PATH}/SPIRV-Tools)

# glslang
set(ENABLE_CTEST OFF CACHE BOOL "Disable glslang tests" FORCE)
add_subdirectory(${EXTERNAL_PATH}/glslang)

# shaderc
set(SHADERC_SKIP_TESTS ON CACHE BOOL "Skip shaderc tests" FORCE)
set(SHADERC_SKIP_EXAMPLES ON CACHE BOOL "Skip shaderc examples" FORCE)
add_subdirectory(${EXTERNAL_PATH}/shaderc)

# Vulkan-ValidationLayers
set(USE_ROBIN_HOOD_HASHING OFF CACHE BOOL "Disable Robin Hood Hashing" FORCE)
add_subdirectory(${EXTERNAL_PATH}/Vulkan-ValidationLayers)

# GLFW
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Skip GLFW examples" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Skip GLFW tests" FORCE)
set(GLFW_BUILD_DOCS  OFF CACHE BOOL "Skip GLFW documentation" FORCE)
add_subdirectory(${EXTERNAL_PATH}/glfw)

# ----- Source -----

# Shaders
set(GLSL_SHADER_PATH ${PROJECT_SOURCE_DIR}/shaders)
set(SPIRV_SHADER_PATH ${GLSL_SHADER_PATH}/_spirv_)
file(GLOB_RECURSE GLSL_SHADERS ${GLSL_SHADER_PATH}/*.vert ${GLSL_SHADER_PATH}/*.frag)
foreach(FILE ${GLSL_SHADERS})
	file(RELATIVE_PATH FILENAME ${GLSL_SHADER_PATH} ${FILE})
	set(SPIRV_FILE ${SPIRV_SHADER_PATH}/${FILENAME}.spv)
	add_custom_command(
		OUTPUT ${SPIRV_FILE}
		COMMAND glslc_exe ${FILE} -o ${SPIRV_FILE}
		COMMENT "Compiling shader: ${FILE}"
		DEPENDS glslc_exe ${FILE}
		VERBATIM
	)
	list(APPEND SPIRV_SHADERS ${SPIRV_FILE})
endforeach()
add_custom_target(shaders ALL DEPENDS ${SPIRV_SHADERS})

# Code
set(SOURCE_PATH ${PROJECT_SOURCE_DIR}/source)
file(GLOB_RECURSE SOURCE_FILES ${SOURCE_PATH}/*.cpp ${SOURCE_PATH}/*.c ${SOURCE_PATH}/*.hpp ${SOURCE_PATH}/*.h)

# Project
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 17)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTERNAL_PATH}/glfw/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTERNAL_PATH}/glm)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTERNAL_PATH}/json/include)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTERNAL_PATH}/stb)
target_include_directories(${PROJECT_NAME} PUBLIC ${EXTERNAL_PATH}/tinygltf)
target_precompile_headers(${PROJECT_NAME} PUBLIC ${SOURCE_PATH}/PCH.hpp)
target_link_libraries(${PROJECT_NAME} vulkan)
target_link_libraries(${PROJECT_NAME} glfw)
add_dependencies(${PROJECT_NAME} shaders)