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
    Renderer();
    ~Renderer();

    void Run();

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
    void RenderNode(Node *node, int16_t cbIndex);

    void HandleInputs();
    
    void CreateNodeDescriptorSet(Node* node);
    void UpdateUniformBuffer(int16_t currentImage);

    void CleanupSwapchain();
    void RecreateSwapchain();

    void GenerateBRDFLUT();
    void GenerateCubemaps();

    bool CheckValidationLayerSupport();
    std::vector<char const*> GetRequiredExtensions();
    bool IsDeviceSuitable(VkPhysicalDevice device);
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device);
    bool HasStencilComponent(VkFormat format);
    SwapChainSupportDetails QuerySwapChainSupport(VkPhysicalDevice device);
    QueueFamilyIndices FindQueueFamilies(VkPhysicalDevice device);
    VkSampleCountFlagBits GetMaxUsableSampleCount();
    VkSurfaceFormatKHR ChooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR ChooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D ChooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkFormat FindSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat FindDepthFormat();
    uint32_t FindMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void UpdateCameraAspectRatio();

    VkCommandBuffer BeginSingleTimeCommands();
    void EndSingleTimeCommands(VkCommandBuffer commandBuffer);

    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
    void DestroyDebugUtilsMessengerEXT();

public:
    VkShaderModule CreateShaderModule(const std::vector<char>& code);
    void CreateImage(uint16_t width, uint16_t height, uint16_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void CreateImageCube(uint16_t width, uint16_t height, uint16_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    VkImageView CreateImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint16_t mipLevels);
    VkImageView CreateImageViewCube(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint16_t mipLevels);
    void TransitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint16_t mipLevels, uint16_t layers = 1);
    void CopyBufferToImage(VkBuffer buffer, VkImage image, uint16_t width, uint16_t height);
    void CopyBufferToImageCube(VkBuffer buffer, VkImage image, uint16_t width, uint16_t height, uint16_t mipLevels, VkDeviceSize faceOffset);
    void CreateBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void GenerateMipmaps(VkImage image, VkFormat imageFormat, uint16_t texWidth, uint16_t texHeight, uint16_t mipLevels);
    void GenerateMipmapsCube(VkImage image, VkFormat imageFormat, uint16_t texWidth, uint16_t texHeight, uint16_t mipLevels);
    void CopyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};
