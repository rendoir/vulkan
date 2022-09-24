#pragma once


// Standard library
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#define _USE_MATH_DEFINES
#include <math.h>
#include <optional>
#include <set>
#include <stdexcept>
#include <string>
#include <type_traits>
#include <vector>


// External
#define NOMINMAX

#include <boost/preprocessor.hpp>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_QUAT_DATA_XYZW
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

#include <vulkan/vulkan.h>

#include <vk_mem_alloc.h>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include <entt/entt.hpp>

#include <stb_image.h>

#include <nlohmann/json.hpp>

#define TINYGLTF_NO_INCLUDE_STB_IMAGE
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define TINYGLTF_NO_INCLUDE_JSON
#include <tiny_gltf.h>
