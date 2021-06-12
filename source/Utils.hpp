#pragma once


#define VK_CHECK_RESULT(returnValue)																				\
{																										\
	VkResult const result = (returnValue);																					\
	if (result != VK_SUCCESS)																				\
	{																									\
		std::cout << "Fatal : VkResult is \"" << result << "\" in " << __FILE__ << " at line " << __LINE__ << std::endl; \
		assert(result == VK_SUCCESS);																		\
	}																									\
}

struct Image
{
    VkImage m_image = VK_NULL_HANDLE;
    VkImageView m_view = VK_NULL_HANDLE;
    VkDeviceMemory m_memory = VK_NULL_HANDLE;
};

struct Settings
{
    uint16_t m_width  = 800;
    uint16_t m_height = 600;
    uint16_t m_maxFramesInFlight = 2;
    bool m_validationLayers = false;
    bool m_fullscreen = false;
    bool m_vsync = false;
    bool m_multiSampling = true;
    VkSampleCountFlagBits m_sampleCount = VK_SAMPLE_COUNT_1_BIT;

    uint16_t m_brdflutSize    = 512;
    uint16_t m_prefilterSize  = 512;
    uint16_t m_irradianceSize = 64;

    uint16_t m_prefilterSamples  = 32;
    float m_irradiancePhiSteps   = 180.0f;
    float m_irradianceThetaSteps = 64.0f;
};

extern Settings g_settings;

struct Buffer
{
	VkDevice m_device = VK_NULL_HANDLE;
	VkBuffer m_buffer = VK_NULL_HANDLE;
	VkDeviceMemory m_memory = VK_NULL_HANDLE;
	VkDescriptorBufferInfo m_descriptor;
	int32_t m_count = 0;
	void* m_mapped = nullptr;
};

std::vector<char const*> const g_validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

std::vector<char const*> const g_deviceExtensions = {
    VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

struct QueueFamilyIndices
{
    std::optional<uint32_t> m_graphicsFamily;
    std::optional<uint32_t> m_presentFamily;

    bool IsComplete() const;
};

struct SwapChainSupportDetails
{
    VkSurfaceCapabilitiesKHR m_capabilities;
    std::vector<VkSurfaceFormatKHR> m_formats;
    std::vector<VkPresentModeKHR> m_presentModes;
};

struct UBOMatrices
{
    glm::mat4 m_projection;
    glm::mat4 m_model;
    glm::mat4 m_view;
    glm::vec3 m_cameraPosition;
};

struct SkyboxMatrices
{
    glm::mat4 m_projection;
    glm::mat4 m_model;
};

struct PushConstantBlockMaterial
{
    glm::vec4 m_baseColorFactor;
    glm::vec4 m_emissiveFactor;
    int m_colorTextureSet;
    int m_physicalTextureSet;
    int m_normalTextureSet;
    int m_occlusionTextureSet;
    int m_emissiveTextureSet;
    float m_metallicFactor;
    float m_roughnessFactor;
    float m_alphaMask;
    float m_alphaMaskCutoff;
};

struct Transform
{
    glm::vec3 m_position = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 m_scale    = glm::vec3(1.0f, 1.0f, 1.0f);
};

namespace Utils
{
	std::string ReadFile(std::string const& filename);
}
