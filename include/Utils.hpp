#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vulkan/vulkan.h>

#include <optional>
#include <string>
#include <vector>

#define VK_CHECK_RESULT(f)																				\
{																										\
	VkResult res = (f);																					\
	if (res != VK_SUCCESS)																				\
	{																									\
		std::cout << "Fatal : VkResult is \"" << res << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; \
		assert(res == VK_SUCCESS);																		\
	}																									\
}

struct Image {
    VkImage image;
    VkImageView view;
    VkDeviceMemory memory;
};

struct Settings {
    uint32_t width = 800;
    uint32_t height = 600;
    uint32_t maxFramesInFlight = 2;
    bool validationLayers = true;
    bool fullscreen = false;
    bool vsync = false;
    bool multiSampling = true;
    VkSampleCountFlagBits sampleCount = VK_SAMPLE_COUNT_1_BIT;

    uint32_t brdflutSize = 512;
    uint32_t prefilterSize = 512;
    uint32_t irradianceSize = 64;

    uint32_t prefilterSamples = 32;
    float irradiancePhiSteps = 180.0f;
    float irradianceThetaSteps = 64.0f;
};

extern Settings settings;

struct Buffer {
	VkDevice device;
	VkBuffer buffer = VK_NULL_HANDLE;
	VkDeviceMemory memory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo descriptor;
	int32_t count = 0;
	void *mapped = nullptr;
};

const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

const std::vector<const char*> deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value() && presentFamily.has_value();
    }
};

struct SwapChainSupportDetails {
    VkSurfaceCapabilitiesKHR capabilities;
    std::vector<VkSurfaceFormatKHR> formats;
    std::vector<VkPresentModeKHR> presentModes;
};

struct UBOMatrices {
    glm::mat4 projection;
    glm::mat4 model;
    glm::mat4 view;
    glm::vec3 camPos;
};

struct SkyboxMatrices {
    glm::mat4 projection;
    glm::mat4 model;
};

struct PushConstBlockMaterial {
    glm::vec4 baseColorFactor;
    glm::vec4 emissiveFactor;
    int colorTextureSet;
    int physicalTextureSet;
    int normalTextureSet;
    int occlusionTextureSet;
    int emissiveTextureSet;
    float metallicFactor;
    float roughnessFactor;
    float alphaMask;
    float alphaMaskCutoff;
};

struct Transform {
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale    = glm::vec3(1.0f, 1.0f, 1.0f);
};

std::vector<char> readFile(const std::string& filename);
