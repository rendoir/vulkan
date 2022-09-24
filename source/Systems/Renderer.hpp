#pragma once

#include <Resources/ImageResource.hpp>
#include <Systems/RenderGraph.hpp>
#include <Systems/System.hpp>
#include <Utilities/Helpers.hpp>
#include <Utilities/Singleton.hpp>

class SwapchainObserver;

class Renderer : public System, public Singleton<Renderer>
{
public: 
    struct RenderSettings
    {
        uint32_t m_width = 1280;
        uint32_t m_height = 720;
        bool m_useAnisotropy = true;
        bool m_useVSync = true;
        bool m_useValidationLayers = true;
        bool m_useMultisampling = true;
        bool m_useSampleShading = true;
        VkSampleCountFlagBits m_rasterizationSampleCount = VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM;
        std::vector<char const*> m_validationLayers{ "VK_LAYER_KHRONOS_validation" };
        std::vector<char const*> m_deviceExtensions{ VK_KHR_SWAPCHAIN_EXTENSION_NAME };
        static constexpr uint16_t m_maxFramesInFlight = 2;
    };

    struct PhysicalDeviceInfo 
    {
        std::optional<uint32_t> m_graphicsQueueFamily;
        std::optional<uint32_t> m_presentationQueueFamily;
        VkSurfaceCapabilitiesKHR m_capabilities;
        VkPhysicalDeviceFeatures2 m_features;
        VkPhysicalDeviceDynamicRenderingFeatures m_dynamicRenderingFeature;
        VkPhysicalDeviceProperties m_properties;
        VkPhysicalDeviceMemoryProperties m_memoryProperties;
        std::vector<VkSurfaceFormatKHR> m_surfaceFormats;
        std::vector<VkPresentModeKHR> m_presentModes;
    };

public:
    virtual void Init() override;
    virtual void PostInit() override;
    virtual void PreTerminate() override;
    virtual void Terminate() override;
    virtual void Update() override;
    bool ShouldExit() const;

    // Getters
    inline VkDevice GetDevice() const { return m_device; }
    inline VmaAllocator GetAllocator() const { return m_allocator; }
    inline RenderGraph const* GetRenderGraph() const { return &m_renderGraph; }
    inline RenderGraph* GetRenderGraph() { return &m_renderGraph; }
    inline ImageResource& GetCurrentSwapchainImage() { return m_swapchainImages[m_currentImageIndex]; }
    inline uint32_t GetNumberOfSwapchainImages() const { return m_swapchainImages.size(); }
    inline uint16_t GetCurrentFrame() const { return m_currentFrame; }
    inline VkFormat GetSwapchainFormat() const { return m_swapchainImageFormat.format; }
    inline VkExtent2D GetSwapchainExtent() const { return m_swapchainExtent; }
    inline VkPipelineCache GetPipelineCache() const { return m_pipelineCache; }
    inline void GetMouseCursorPosition(double& xPosition, double& yPosition) const { glfwGetCursorPos(m_window, &xPosition, &yPosition); }
    inline int32_t GetMouseButton(int32_t button) const { return glfwGetMouseButton(m_window, button); }
    inline int32_t GetKey(int32_t key) const { return glfwGetKey(m_window, key); }
    inline PhysicalDeviceInfo const& GetPhysicalDeviceInfo() const { return m_physicalDeviceInfo; }
    inline RenderSettings const& GetRenderSettings() const { return m_renderSettings; }
    inline std::vector<ImageResource> const& GetSwapchainImages() const { return m_swapchainImages; }
    VkSampleCountFlagBits GetRasterizationSampleCount() const;
    VkFormat ChooseDepthFormat(bool requireStencil = false) const;
    VkFormat ChooseBackbufferFormat() const;
    static VkImageAspectFlags GetAspectFlagsFromFormat(VkFormat format);
    static bool FormatHasStencil(VkFormat format);
    static void GetAccessMasksForLayoutTransition(VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags& srcAccessMask, VkAccessFlags& dstAccessMask);

    // Observers
    inline void AddSwapchainObserver(SwapchainObserver* observer) { m_swapchainObservers.push_back(observer); }
    inline void RemoveSwapchainObserver(SwapchainObserver* observer) { EraseFirstMatch(m_swapchainObservers, observer); }

    // Helpers
    VkCommandBuffer BeginSingleUseCommandBuffer();
    void EndSingleUseCommandBuffer(VkCommandBuffer commandBuffer);

private:
    // Core
    void RenderFrame();

    // Create
    void CreateNativeWindow();
    void CreateInstance();
    void CreateDebugMessenger();
    void CreateSurface();
    void PickPhysicalDevice();
    void CreateLogicalDevice();
    void CreateMemoryAllocator();
    void CreateSwapchain();
    void CreateSyncObjects();
    void CreatePipelineCache();
    void CreateSingleUseCommandPool();
    void RecreateSwapchain();

    // Destroy
    void DestroyNativeWindow();
    void DestroyInstance();
    void DestroyDebugMessenger();
    void DestroySurface();
    void DestroyDevice();
    void DestroyMemoryAllocator();
    void DestroySwapchain();
    void DestroySyncObjects();
    void DestroyPipelineCache();
    void DestroySingleUseCommandPool();

    // Helpers
    std::vector<char const*> GetRequiredExtensions() const;
    bool CheckValidationLayerSupport() const;
    bool IsPhysicalDeviceSuitable(VkPhysicalDevice device, PhysicalDeviceInfo& deviceInfo) const;
    bool CheckQueueFamilySupport(VkPhysicalDevice device, PhysicalDeviceInfo& deviceInfo) const;
    bool CheckDeviceExtensionSupport(VkPhysicalDevice device, PhysicalDeviceInfo& deviceInfo) const;
    bool CheckSwapchainSupport(VkPhysicalDevice device, PhysicalDeviceInfo& deviceInfo) const;
    bool CheckPhysicalDeviceProperties(VkPhysicalDevice device, PhysicalDeviceInfo& deviceInfo) const;
    void SelectBestPhysicalDevice(VkPhysicalDevice device, PhysicalDeviceInfo const& deviceInfo);
    void PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugInfo) const;
    VkSurfaceFormatKHR ChooseSwapchainSurfaceFormat() const;
    VkPresentModeKHR ChooseSwapchainPresentMode() const;
    VkExtent2D ChooseSwapchainExtent() const;
    VkFormat FindSupportedFormat(std::vector<VkFormat> const& sortedCandidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;

    // Callbacks
    static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, VkDebugUtilsMessengerCallbackDataEXT const* pCallbackData, void* pUserData);
    static void FramebufferResizeCallback(GLFWwindow* window, int width, int height);

private:
    PresentationRenderGraph m_renderGraph;
    RenderSettings m_renderSettings;
    PhysicalDeviceInfo m_physicalDeviceInfo;

    GLFWwindow* m_window = nullptr;

    VkInstance m_instance = VK_NULL_HANDLE;
    VkDevice m_device = VK_NULL_HANDLE;
    VkPhysicalDevice m_physicalDevice = VK_NULL_HANDLE;
    VmaAllocator m_allocator = VK_NULL_HANDLE;

    VkSurfaceKHR m_surface = VK_NULL_HANDLE;
    VkSwapchainKHR m_swapchain = VK_NULL_HANDLE;
    VkSurfaceFormatKHR m_swapchainImageFormat;
    VkExtent2D m_swapchainExtent = { 0, 0 };
    std::vector<ImageResource> m_swapchainImages;
    std::vector<SwapchainObserver*> m_swapchainObservers;
    bool m_framebufferResized = false;
    
    VkQueue m_graphicsQueue = VK_NULL_HANDLE;
    VkQueue m_presentQueue = VK_NULL_HANDLE;
    VkCommandPool m_singleUseCommandPool = VK_NULL_HANDLE;

    std::vector<VkSemaphore> m_imageAvailableSemaphores;
    std::vector<VkSemaphore> m_renderFinishedSemaphores;
    std::vector<VkFence> m_inFlightFences;
    std::vector<VkFence> m_fencesPerImageInFlight;
    uint16_t m_currentFrame = 0;
    uint32_t m_currentImageIndex = 0;

    VkPipelineCache m_pipelineCache = VK_NULL_HANDLE;
    
    VkDebugUtilsMessengerEXT m_debugMessenger = VK_NULL_HANDLE;

    friend class Singleton<Renderer>;
};
