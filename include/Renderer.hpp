#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "Utils.hpp"

#include <vector>

class Skybox;
struct Model;
struct Node;
struct Texture;
struct Texture3D;
class Camera;
class CameraControl;

class Renderer {
public:
    Renderer();
    ~Renderer();

    void run();

public:
    GLFWwindow* window;
    VkSurfaceKHR surface;

    VkInstance instance;
    VkDebugUtilsMessengerEXT debugMessenger;

    VkDevice device;
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;

    VkQueue graphicsQueue;
    VkQueue presentQueue;
    QueueFamilyIndices queueFamilyIndices;

    VkSwapchainKHR swapChain;
    std::vector<VkImage> swapChainImages;
    VkFormat swapChainImageFormat;
    VkExtent2D swapChainExtent;
    std::vector<VkImageView> swapChainImageViews;
    std::vector<VkFramebuffer> swapChainFramebuffers;

    VkRenderPass renderPass;

    VkCommandPool commandPool;

    Image colorMultisample;
    Image depthStencil;
    VkFormat depthFormat;

    VkDescriptorPool descriptorPool;

    std::vector<VkCommandBuffer> commandBuffers;

    std::vector<VkSemaphore> imageAvailableSemaphores;
    std::vector<VkSemaphore> renderFinishedSemaphores;
    std::vector<VkFence> inFlightFences;
    size_t currentFrame = 0;

    bool framebufferResized = false;


    // Scene
    Model *model;
    Model *cube;
    Skybox *skybox;
    Texture *empty;
    Texture *brdflut;
    Texture3D *environmentCube;
    Texture3D *irradianceCube;
    Texture3D *prefilteredCube;
    Camera *camera;
    CameraControl *cameraControl;

    VkPipelineLayout pipelineLayout;
    VkPipeline graphicsPipeline;

    struct {
        VkDescriptorSetLayout scene;
        VkDescriptorSetLayout material;
        VkDescriptorSetLayout node;
    } descriptorSetLayouts;
    std::vector<VkDescriptorSet> descriptorSets;

    UBOMatrices uboMatrices;
    PushConstBlockMaterial pushConstBlockMaterial;
    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;


private:
    void initWindow();
    void createInstance();
    void setupDebugMessenger();
    void createSurface();
    void pickPhysicalDevice();
    void createLogicalDevice();
    void createSwapChain();
    void createImageViews();
    void createRenderPass();
    void createCommandPool();
    void createCommandBuffers();
    void createColorResources();
    void createDepthResources();
    void createFramebuffers();
    void createSyncObjects();
    void loadAssets();
    void initCamera();
    void createUniformBuffers();
    void createDescriptors();
    void createPipeline();
    void recordCommandBuffers();
    void initSkybox();

    void renderFrame();
    void handleInputs();
    void renderNode(Node *node, uint32_t cbIndex);
    void createNodeDescriptorSet(Node* node);
    void updateUniformBuffer(uint32_t currentImage);

    void cleanupSwapchain();
    void recreateSwapchain();

    void generateBRDFLUT();
    void generateCubemaps();

    bool checkValidationLayerSupport();
    std::vector<const char*> getRequiredExtensions();
    bool isDeviceSuitable(VkPhysicalDevice device);
    bool checkDeviceExtensionSupport(VkPhysicalDevice device);
    SwapChainSupportDetails querySwapChainSupport(VkPhysicalDevice device);
    QueueFamilyIndices findQueueFamilies(VkPhysicalDevice device);
    VkSampleCountFlagBits getMaxUsableSampleCount();
    VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);
    VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR>& availablePresentModes);
    VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);
    VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features);
    VkFormat findDepthFormat();
    uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties);
    void updateCameraAspectRatio();
    VkCommandBuffer beginSingleTimeCommands();
    void endSingleTimeCommands(VkCommandBuffer commandBuffer);

    static bool hasStencilComponent(VkFormat format);
    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);
    static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
    static void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo);
    
    VkResult CreateDebugUtilsMessengerEXT(const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo);
    void DestroyDebugUtilsMessengerEXT();

public:
    VkShaderModule createShaderModule(const std::vector<char>& code);
    void createImage(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    void createImageCube(uint32_t width, uint32_t height, uint32_t mipLevels, VkSampleCountFlagBits numSamples, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    VkImageView createImageViewCube(VkImage image, VkFormat format, VkImageAspectFlags aspectFlags, uint32_t mipLevels);
    void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, uint32_t mipLevels, uint32_t layers = 1);
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
    void copyBufferToImageCube(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height, uint32_t mipLevels, VkDeviceSize faceOffset);
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);
    void generateMipmaps(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    void generateMipmapsCube(VkImage image, VkFormat imageFormat, int32_t texWidth, int32_t texHeight, uint32_t mipLevels);
    void copyBuffer(VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);
};
