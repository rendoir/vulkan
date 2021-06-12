#pragma once

#include "Utils.hpp"

class Skybox;
struct Model;
struct Node;
struct Texture2D;
struct Texture3D;
class Camera;
class CameraControl;

class Renderer
{
public:
	static Renderer& GetInstance();

    void Init();
	void Run();

private:
	Renderer();
	~Renderer();

public:
    GLFWwindow*  m_window = nullptr;
    VkSurfaceKHR m_surface = VK_NULL_HANDLE;

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDevice   m_device   = VK_NULL_HANDLE;

    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue  = VK_NULL_HANDLE;
    QueueFamilyIndices m_queueFamilyIndices;

    VkSwapchainKHR m_swapChain = VK_NULL_HANDLE;
    VkFormat   m_swapChainImageFormat;
    VkExtent2D m_swapChainExtent;
    std::vector<VkImage>       m_swapChainImages;
    std::vector<VkImageView>   m_swapChainImageViews;
    std::vector<VkFramebuffer> m_swapChainFramebuffers;

    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkCommandPool    m_commandPool    = VK_NULL_HANDLE;

    Image m_colorMultisample;
    Image m_depthStencil;
    VkFormat m_depthFormat;

    std::vector<VkCommandBuffer> m_commandBuffers;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence>     m_inFlightFences;
    uint8_t m_currentFrame = 0;

    bool m_framebufferResized = false;


    // Scene
    Model* m_model = nullptr;
    Model* m_cube  = nullptr;
    Skybox* m_skybox = nullptr;
    Texture2D* m_empty   = nullptr;
    Texture2D* m_brdflut = nullptr;
    Texture3D* m_environmentCube = nullptr;
    Texture3D* m_irradianceCube  = nullptr;
    Texture3D* m_prefilteredCube = nullptr;
    Camera* m_camera = nullptr;
    CameraControl* m_cameraControl = nullptr;

    VkRenderPass m_renderPass = VK_NULL_HANDLE;
    VkPipeline   m_graphicsPipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    struct {
        VkDescriptorSetLayout m_scene    = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_material = VK_NULL_HANDLE;
        VkDescriptorSetLayout m_node     = VK_NULL_HANDLE;
    } m_descriptorSetLayouts;
    std::vector<VkDescriptorSet> m_descriptorSets;

    UBOMatrices m_uboMatrices;
    PushConstantBlockMaterial m_pushConstBlockMaterial;
    std::vector<VkBuffer> m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;


private:
    void InitWindow();
    void CreateInstance();
    void SetupDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateSwapChain();
    void CreateImageViews();
    void CreateRenderPass();
    void CreateCommandPool();
    void CreateCommandBuffers();
    void CreateColorResources();
    void CreateDepthResources();
    void CreateFramebuffers();
    void CreateSyncObjects();
    void LoadAssets();
    void InitCamera();
    void CreateUniformBuffers();
    void CreateDescriptors();
    void CreatePipeline();
    void RecordCommandBuffers();
    void InitSkybox();

    void RenderFrame();
    void RenderNode(Node const* node, int16_t const cbIndex);

    void HandleInputs();
    
    void CreateNodeDescriptorSet(Node* node);
    void UpdateUniformBuffer(int16_t currentImage);

    void CleanupSwapchain();
    void RecreateSwapchain();

    void GenerateBRDFLUT();
    void GenerateCubemaps();

    bool CheckValidationLayerSupport();
    std::vector<char const*> GetRequiredExtensions();
    bool IsDeviceSuitable(VkPhysicalDevice const device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice const device);
    bool HasStencilComponent(VkFormat const format);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice const device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice const device);
    VkSampleCountFlagBits GetMaxUsableSampleCount();
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(std::vector<VkSurfaceFormatKHR> const& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(std::vector<VkPresentModeKHR> const& availablePresentModes);
    VkExtent2D ChooseSwapExtent(VkSurfaceCapabilitiesKHR const& capabilities);
    VkFormat FindSupportedFormat(std::vector<VkFormat> const& candidates, VkImageTiling const tiling, VkFormatFeatureFlags const features);
    VkFormat FindDepthFormat();
    uint32_t FindMemoryType(uint32_t const typeFilter, VkMemoryPropertyFlags const properties);
    void UpdateCameraAspectRatio();

    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData, void* pUserData);
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    VkResult CreateDebugUtilsMessengerEXT(VkDebugUtilsMessengerCreateInfoEXT const* pCreateInfo);
    void DestroyDebugUtilsMessengerEXT();

public:
    VkShaderModule CreateShaderModule(std::string const& code);
    void CreateImage(uint16_t const width, uint16_t const height, uint16_t const mipLevels, VkSampleCountFlagBits const numSamples, VkFormat const format, VkImageTiling const tiling, VkImageUsageFlags const usage, VkMemoryPropertyFlags const properties, VkImage& image, VkDeviceMemory& imageMemory);
    void CreateImageCube(uint16_t const width, uint16_t const height, uint16_t const mipLevels, VkSampleCountFlagBits const numSamples, VkFormat const format, VkImageTiling const tiling, VkImageUsageFlags const usage, VkMemoryPropertyFlags const properties, VkImage& image, VkDeviceMemory& imageMemory);
    VkImageView CreateImageView(VkImage const image, VkFormat const format, VkImageAspectFlags const aspectFlags, uint16_t const mipLevels);
    VkImageView CreateImageViewCube(VkImage const image, VkFormat const format, VkImageAspectFlags const aspectFlags, uint16_t const mipLevels);
    void TransitionImageLayout(VkImage image, VkFormat const format, VkImageLayout const oldLayout, VkImageLayout const newLayout, uint16_t const mipLevels, uint16_t const layers = 1);
    void CopyBufferToImage(VkBuffer const buffer, VkImage image, uint16_t const width, uint16_t const height);
    void CopyBufferToImageCube(VkBuffer const buffer, VkImage image, uint16_t const width, uint16_t const height, uint16_t const mipLevels, VkDeviceSize const faceOffset);
    void CreateBuffer(VkDeviceSize const size, VkBufferUsageFlags const usage, VkMemoryPropertyFlags const properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void GenerateMipmaps(VkImage image, VkFormat const imageFormat, uint16_t const texWidth, uint16_t const texHeight, uint16_t const mipLevels);
    void GenerateMipmapsCube(VkImage image, VkFormat const imageFormat, uint16_t const texWidth, uint16_t const texHeight, uint16_t const mipLevels);
    void CopyBuffer(VkBuffer const srcBuffer, VkBuffer dstBuffer, VkDeviceSize const size);

	Renderer(Renderer const&) = delete;
	Renderer(Renderer&&) = delete;
	Renderer& operator=(Renderer const&) = delete;
	Renderer& operator=(Renderer&&) = delete;
};
