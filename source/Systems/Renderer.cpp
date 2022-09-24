#include <Systems/Renderer.hpp>

#include <Resources/ImageResource.hpp>
#include <Systems/RenderGraph.hpp>
#include <Systems/RenderPasses/BrdflutPass.hpp>
#include <Systems/RenderPasses/IrradiancePass.hpp>
#include <Systems/RenderPasses/PrefilterPass.hpp>
#include <Systems/RenderPasses/ShadingPass.hpp>
#include <Systems/RenderPasses/SkyboxPass.hpp>
#include <Utilities/Observer.hpp>
#include <Utilities/Helpers.hpp>

static const std::vector<VkFormat> s_sortedDepthFormats = {
    VK_FORMAT_D32_SFLOAT_S8_UINT,
    VK_FORMAT_D32_SFLOAT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D16_UNORM_S8_UINT,
    VK_FORMAT_D16_UNORM
};

static const std::vector<VkFormat> s_sortedDepthStencilFormats = {
    VK_FORMAT_D32_SFLOAT_S8_UINT,
    VK_FORMAT_D24_UNORM_S8_UINT,
    VK_FORMAT_D16_UNORM_S8_UINT
};

static const std::vector<VkFormat> s_sortedBackbufferFormats = {
    VK_FORMAT_R64G64B64A64_SFLOAT,
    VK_FORMAT_R32G32B32A32_SFLOAT,
    VK_FORMAT_R16G16B16A16_SFLOAT,
    VK_FORMAT_R16G16B16A16_UNORM,
    VK_FORMAT_R8G8B8A8_UNORM
};

void Renderer::Init()
{
    CreateNativeWindow();
    CreateInstance();
    CreateDebugMessenger();
    CreateSurface();
    PickPhysicalDevice();
    CreateLogicalDevice();
    CreateMemoryAllocator();
    CreateSwapchain();
    CreateSyncObjects();
    CreatePipelineCache();
    CreateSingleUseCommandPool();
}

void Renderer::PostInit()
{
    m_renderGraph.AddPass<BrdflutPass>("brdflut");
    m_renderGraph.AddPass<IrradiancePass>("irradiance");
    m_renderGraph.AddPass<PrefilterPass>("prefilter");
    m_renderGraph.AddPass<SkyboxPass>("skybox");
    m_renderGraph.AddPass<ShadingPass>("shading");
    m_renderGraph.Init();
}

void Renderer::PreTerminate()
{
    vkDeviceWaitIdle(m_device);

    m_renderGraph.Terminate();
}

void Renderer::Terminate()
{
    DestroySingleUseCommandPool();
    DestroyPipelineCache();
    DestroySyncObjects();
    DestroySwapchain();
    DestroyMemoryAllocator();
    DestroyDevice();
    DestroySurface();
    DestroyDebugMessenger();
    DestroyInstance();
    DestroyNativeWindow();
}

void Renderer::RecreateSwapchain()
{
    int32_t width = 0, height = 0;
    glfwGetFramebufferSize(m_window, &width, &height);
    while (width == 0 || height == 0)
    {
        // Wait until the window is no longer minimized
        glfwGetFramebufferSize(m_window, &width, &height);
        glfwWaitEvents();
    }

    vkDeviceWaitIdle(m_device);

    DestroySwapchain();
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(m_physicalDevice, m_surface, &m_physicalDeviceInfo.m_capabilities);
    CreateSwapchain();

    for (SwapchainObserver* observer : m_swapchainObservers)
    {
        observer->OnSwapchainRecreated(m_swapchainExtent);
    }
}

void Renderer::Update()
{
    glfwPollEvents();
    RenderFrame();
}

bool Renderer::ShouldExit() const
{
    if (!m_window)
    {
        return true;
    }

    return glfwWindowShouldClose(m_window);
}

void Renderer::CreateNativeWindow()
{
    glfwInit();
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

    m_window = glfwCreateWindow(m_renderSettings.m_width, m_renderSettings.m_height, "Roar", nullptr, nullptr);

    if (!m_window)
    {
        ThrowError("Failed to create window.");
    }

    glfwSetWindowUserPointer(m_window, this);
    glfwSetFramebufferSizeCallback(m_window, FramebufferResizeCallback);
}

void Renderer::DestroyNativeWindow()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
    }
    
    glfwTerminate();
}

void Renderer::CreateInstance()
{
    if (m_renderSettings.m_useValidationLayers)
    {
        SetEnvVariable("VK_LAYER_PATH", VK_LAYER_PATH);

        if (!CheckValidationLayerSupport())
        {
            ThrowError("Validation layers requested, but not available.");
        }
    }

    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Roar";
    appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.pEngineName = "Roar";
    appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
    appInfo.apiVersion = VK_API_VERSION_1_3;

    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &appInfo;

    std::vector<char const*> extensions = GetRequiredExtensions();
    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
    instanceInfo.ppEnabledExtensionNames = extensions.data();

    if (m_renderSettings.m_useValidationLayers)
    {
        VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
        PopulateDebugMessengerCreateInfo(debugMessengerInfo);

        VkValidationFeatureEnableEXT enables[] = {VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT};
        VkValidationFeaturesEXT features = {};
        features.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        features.enabledValidationFeatureCount = 1;
        features.pEnabledValidationFeatures = enables;

        instanceInfo.enabledLayerCount = static_cast<uint32_t>(m_renderSettings.m_validationLayers.size());
        instanceInfo.ppEnabledLayerNames = m_renderSettings.m_validationLayers.data();
        
        PNextChainPushBack(&instanceInfo, &features);
        PNextChainPushBack(&instanceInfo, &debugMessengerInfo);
    }
    else
    {
        instanceInfo.enabledLayerCount = 0;
        instanceInfo.pNext = nullptr;
    }

    if (vkCreateInstance(&instanceInfo, nullptr, &m_instance) != VK_SUCCESS)
    {
        ThrowError("Failed to create instance.");
    }
}

void Renderer::DestroyInstance()
{
    vkDestroyInstance(m_instance, nullptr);
}

void Renderer::CreateDebugMessenger()
{
    if (!m_renderSettings.m_useValidationLayers)
    {
        return;
    }

    auto debugCreateFunction = (PFN_vkCreateDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_instance, "vkCreateDebugUtilsMessengerEXT");
    if (!debugCreateFunction)
    {
        ThrowError("Failed to retrieve the create debug messenger function pointer.");
    }

    VkDebugUtilsMessengerCreateInfoEXT debugMessengerInfo = {};
    PopulateDebugMessengerCreateInfo(debugMessengerInfo);

    if (debugCreateFunction(m_instance, &debugMessengerInfo, nullptr, &m_debugMessenger) != VK_SUCCESS)
    {
        ThrowError("Failed to create the debug messenger.");
    }
}

void Renderer::DestroyDebugMessenger()
{
    if (!m_renderSettings.m_useValidationLayers)
    {
        return;
    }

    auto debugDestroyFunction = (PFN_vkDestroyDebugUtilsMessengerEXT) vkGetInstanceProcAddr(m_instance, "vkDestroyDebugUtilsMessengerEXT");
    if (!debugDestroyFunction)
    {
        ThrowError("Failed to retrieve the destroy debug messenger function pointer.");
        return;
    }

    debugDestroyFunction(m_instance, m_debugMessenger, nullptr);
}

void Renderer::CreateSurface()
{
    if (glfwCreateWindowSurface(m_instance, m_window, nullptr, &m_surface) != VK_SUCCESS)
    {
        ThrowError("Failed to create window surface.");
    }
}

void Renderer::DestroySurface()
{
    vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
}

void Renderer::PickPhysicalDevice()
{
    uint32_t deviceCount = 0;
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, nullptr);

    if (deviceCount == 0)
    {
        ThrowError("Failed to find a physical device with Vulkan support.");
    }

    std::vector<VkPhysicalDevice> devices(deviceCount);
    vkEnumeratePhysicalDevices(m_instance, &deviceCount, devices.data());

    for (VkPhysicalDevice const& device : devices)
    {
        PhysicalDeviceInfo deviceInfo;
        if (!IsPhysicalDeviceSuitable(device, deviceInfo))
        {
            continue;
        }

        SelectBestPhysicalDevice(device, deviceInfo);
    }

    if (m_physicalDevice == VK_NULL_HANDLE)
    {
        ThrowError("Failed to find a suitable physical device.");
    }
}

void Renderer::CreateLogicalDevice()
{
    std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
    std::set<uint32_t> const uniqueQueueFamilies = { m_physicalDeviceInfo.m_graphicsQueueFamily.value(), m_physicalDeviceInfo.m_presentationQueueFamily.value() };

    float const queuePriority = 1.0f;
    for (uint32_t queueFamily : uniqueQueueFamilies)
    {
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        queueCreateInfos.push_back(queueCreateInfo);
    }

    VkPhysicalDeviceFeatures2 deviceFeatures = {};
    deviceFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    deviceFeatures.features.samplerAnisotropy = m_renderSettings.m_useAnisotropy;
    deviceFeatures.features.sampleRateShading = m_renderSettings.m_useSampleShading;

    VkPhysicalDeviceDynamicRenderingFeatures dynamicRenderingFeature = {};
    dynamicRenderingFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;
    dynamicRenderingFeature.dynamicRendering = VK_TRUE;
    PNextChainPushBack(&deviceFeatures, &dynamicRenderingFeature);

    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

    deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
    deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(m_renderSettings.m_deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = m_renderSettings.m_deviceExtensions.data();

    deviceCreateInfo.pNext = &deviceFeatures;
    
    if (m_renderSettings.m_useValidationLayers)
    {
        deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(m_renderSettings.m_validationLayers.size());
        deviceCreateInfo.ppEnabledLayerNames = m_renderSettings.m_validationLayers.data();
    }
    else
    {
        deviceCreateInfo.enabledLayerCount = 0;
    }

    if (vkCreateDevice(m_physicalDevice, &deviceCreateInfo, nullptr, &m_device) != VK_SUCCESS)
    {
        ThrowError("Failed to create logical device.");
    }

    vkGetDeviceQueue(m_device, m_physicalDeviceInfo.m_graphicsQueueFamily.value(), 0, &m_graphicsQueue);
    vkGetDeviceQueue(m_device, m_physicalDeviceInfo.m_presentationQueueFamily.value(), 0, &m_presentQueue);
}

void Renderer::DestroyDevice()
{
    vkDestroyDevice(m_device, nullptr);
}

void Renderer::CreateMemoryAllocator()
{
    VmaAllocatorCreateInfo allocatorInfo = {};
    allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
    allocatorInfo.device = m_device;
    allocatorInfo.instance = m_instance;
    allocatorInfo.physicalDevice = m_physicalDevice;

    if (vmaCreateAllocator(&allocatorInfo, &m_allocator) != VK_SUCCESS)
    {
        ThrowError("Failed to create memory allocator.");
    }
}

void Renderer::DestroyMemoryAllocator()
{
    vmaDestroyAllocator(m_allocator);
}

void Renderer::CreateSwapchain()
{
    VkPresentModeKHR const presentMode = ChooseSwapchainPresentMode();
    m_swapchainImageFormat = ChooseSwapchainSurfaceFormat();
    m_swapchainExtent = ChooseSwapchainExtent();

    uint32_t imageCount = m_physicalDeviceInfo.m_capabilities.minImageCount + 1;
    uint32_t const maxImageCount = m_physicalDeviceInfo.m_capabilities.maxImageCount;
    if (maxImageCount > 0 && imageCount > maxImageCount)
    {
        imageCount = maxImageCount;
    }

    VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
    swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainCreateInfo.surface = m_surface;

    swapchainCreateInfo.minImageCount = imageCount;
    swapchainCreateInfo.imageFormat = m_swapchainImageFormat.format;
    swapchainCreateInfo.imageColorSpace = m_swapchainImageFormat.colorSpace;
    swapchainCreateInfo.imageExtent = m_swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;

    std::array<uint32_t, 2> const queueIndices = { m_physicalDeviceInfo.m_graphicsQueueFamily.value(), m_physicalDeviceInfo.m_presentationQueueFamily.value() };

    if (m_physicalDeviceInfo.m_graphicsQueueFamily != m_physicalDeviceInfo.m_presentationQueueFamily)
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainCreateInfo.queueFamilyIndexCount = queueIndices.size();
        swapchainCreateInfo.pQueueFamilyIndices = queueIndices.data();
    }
    else
    {
        swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    }

    swapchainCreateInfo.preTransform = m_physicalDeviceInfo.m_capabilities.currentTransform;
    swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    swapchainCreateInfo.presentMode = presentMode;
    swapchainCreateInfo.clipped = true;

    if (vkCreateSwapchainKHR(m_device, &swapchainCreateInfo, nullptr, &m_swapchain) != VK_SUCCESS)
    {
        ThrowError("Failed to create swap chain.");
    }

    // Swapchain images
    std::vector<VkImage> swapchainImages;
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr);
    swapchainImages.resize(imageCount);
    vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, swapchainImages.data());

    ImageCreateInfo swapchainImageCreateInfo;
    swapchainImageCreateInfo.m_format = m_swapchainImageFormat.format;
    swapchainImageCreateInfo.m_width = m_swapchainExtent.width;
    swapchainImageCreateInfo.m_height = m_swapchainExtent.height;
    swapchainImageCreateInfo.m_sizeType = SizeType::Absolute;
    swapchainImageCreateInfo.m_mipLevels = 1;
    swapchainImageCreateInfo.m_layers = 1;

    for (uint32_t i = 0; i < imageCount; i++)
    {
        ImageResource& image = m_swapchainImages.emplace_back();
        image.SetCreationInfo(swapchainImageCreateInfo);
        image.SetImage(swapchainImages[i]);
        image.CreateImageView();
    }
}

void Renderer::DestroySwapchain()
{
    vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);

    m_swapchainImages.clear();
}

void Renderer::CreateSyncObjects()
{
    m_imageAvailableSemaphores.resize(m_renderSettings.m_maxFramesInFlight);
    m_renderFinishedSemaphores.resize(m_renderSettings.m_maxFramesInFlight);
    m_inFlightFences.resize(m_renderSettings.m_maxFramesInFlight);
    m_fencesPerImageInFlight.resize(m_swapchainImages.size(), VK_NULL_HANDLE);

    VkSemaphoreCreateInfo semaphoreInfo = {};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (uint16_t i = 0; i < m_renderSettings.m_maxFramesInFlight; ++i)
    {
        if (vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]) != VK_SUCCESS ||
            vkCreateSemaphore(m_device, &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]) != VK_SUCCESS ||
            vkCreateFence(m_device, &fenceInfo, nullptr, &m_inFlightFences[i]) != VK_SUCCESS)
        {
            ThrowError("Failed to create synchronization object.");
        }
    }
}

void Renderer::DestroySyncObjects()
{
    for (uint16_t i = 0; i < m_renderSettings.m_maxFramesInFlight; i++)
    {
        vkDestroySemaphore(m_device, m_renderFinishedSemaphores[i], nullptr);
        vkDestroySemaphore(m_device, m_imageAvailableSemaphores[i], nullptr);
        vkDestroyFence(m_device, m_inFlightFences[i], nullptr);
    }

    m_renderFinishedSemaphores.clear();
    m_imageAvailableSemaphores.clear();
    m_inFlightFences.clear();
    m_fencesPerImageInFlight.clear();
}

void Renderer::CreatePipelineCache()
{
	VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
	pipelineCacheCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;

    if (vkCreatePipelineCache(m_device, &pipelineCacheCreateInfo, nullptr, &m_pipelineCache) != VK_SUCCESS)
    {
        ThrowError("Failed to create pipeline cache.");
    }
}

void Renderer::DestroyPipelineCache()
{
	vkDestroyPipelineCache(m_device, m_pipelineCache, nullptr);
}

void Renderer::CreateSingleUseCommandPool()
{
    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
    poolInfo.queueFamilyIndex = m_physicalDeviceInfo.m_graphicsQueueFamily.value();

    if (vkCreateCommandPool(m_device, &poolInfo, nullptr, &m_singleUseCommandPool) != VK_SUCCESS)
    {
        ThrowError("Failed to create command pool.");
    }
}

void Renderer::DestroySingleUseCommandPool()
{
    vkDestroyCommandPool(m_device, m_singleUseCommandPool, nullptr);
}

void Renderer::RenderFrame()
{
    vkWaitForFences(m_device, 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

    VkResult const acquireImageResult = vkAcquireNextImageKHR(m_device, m_swapchain, UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &m_currentImageIndex);

    if (acquireImageResult == VK_ERROR_OUT_OF_DATE_KHR)
    {
        RecreateSwapchain();
        return;
    }
    else if (acquireImageResult != VK_SUCCESS && acquireImageResult != VK_SUBOPTIMAL_KHR)
    {
        ThrowError("Failed to acquire swapchain image.");
    }
    
    if (m_fencesPerImageInFlight[m_currentImageIndex] != VK_NULL_HANDLE)
    {
        vkWaitForFences(m_device, 1, &m_fencesPerImageInFlight[m_currentImageIndex], VK_TRUE, UINT64_MAX);
    }
    m_fencesPerImageInFlight[m_currentImageIndex] = m_inFlightFences[m_currentFrame];

    VkCommandBuffer& currentCommandBuffer = m_renderGraph.GetCommandBuffer(m_currentImageIndex);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

    VkSemaphore waitSemaphores[] = { m_imageAvailableSemaphores[m_currentFrame] };
    VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &currentCommandBuffer;

    VkSemaphore signalSemaphores[] = { m_renderFinishedSemaphores[m_currentFrame] };
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;

    vkResetFences(m_device, 1, &m_inFlightFences[m_currentFrame]);

    m_renderGraph.Execute(currentCommandBuffer);

    if (vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, m_inFlightFences[m_currentFrame]) != VK_SUCCESS)
    {
        ThrowError("Failed to submit command buffers.");
    }

    VkPresentInfoKHR presentInfo = {};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;

    VkSwapchainKHR swapchains[] = { m_swapchain };
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapchains;
    presentInfo.pImageIndices = &m_currentImageIndex;

    VkResult const queuePresentResult = vkQueuePresentKHR(m_presentQueue, &presentInfo);
    if (queuePresentResult == VK_ERROR_OUT_OF_DATE_KHR || queuePresentResult == VK_SUBOPTIMAL_KHR || m_framebufferResized)
    {
        m_framebufferResized = false;
        RecreateSwapchain();
    }
    else if (queuePresentResult != VK_SUCCESS)
    {
        ThrowError("Failed to present swapchain image.");
    }

    m_currentFrame = (m_currentFrame + 1) % m_renderSettings.m_maxFramesInFlight;
}

std::vector<char const*> Renderer::GetRequiredExtensions() const
{
    uint32_t glfwExtensionCount = 0;
    char const** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);

    std::vector<char const*> extensions(glfwExtensions, glfwExtensions + glfwExtensionCount);

    if (m_renderSettings.m_useValidationLayers)
    {
        extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
        extensions.push_back(VK_EXT_VALIDATION_FEATURES_EXTENSION_NAME);
    }

    return extensions;
}

bool Renderer::CheckValidationLayerSupport() const
{
    uint32_t layerCount;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    for (char const* const layerName : m_renderSettings.m_validationLayers)
    {
        auto isSameLayerPredicate = [layerName](VkLayerProperties const& availableLayer) -> bool { return strcmp(layerName, availableLayer.layerName) == 0; };

        if (std::find_if(availableLayers.begin(), availableLayers.end(), isSameLayerPredicate) == availableLayers.end())
        {
            return false;
        }
    }

    return true;
}

bool Renderer::IsPhysicalDeviceSuitable(VkPhysicalDevice device, PhysicalDeviceInfo& deviceInfo) const
{
    return CheckQueueFamilySupport(device, deviceInfo)
        && CheckDeviceExtensionSupport(device, deviceInfo)
        && CheckSwapchainSupport(device, deviceInfo)
        && CheckPhysicalDeviceProperties(device, deviceInfo);
}

bool Renderer::CheckQueueFamilySupport(VkPhysicalDevice device, PhysicalDeviceInfo& deviceInfo) const
{
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    for (uint32_t queueIndex = 0; queueIndex < queueFamilies.size(); ++queueIndex)
    {
        VkQueueFamilyProperties const& queueFamily = queueFamilies[queueIndex];

        if (queueFamily.queueCount == 0)
        {
            continue;
        }

        if (queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT)
        {
            deviceInfo.m_graphicsQueueFamily = queueIndex;
        }

        VkBool32 presentationSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, queueIndex, m_surface, &presentationSupport);

        if (presentationSupport)
        {
            deviceInfo.m_presentationQueueFamily = queueIndex;
        }

        if (deviceInfo.m_graphicsQueueFamily.has_value() && deviceInfo.m_presentationQueueFamily.has_value())
        {
            return true;
        }
    }

    return false;
}

bool Renderer::CheckDeviceExtensionSupport(VkPhysicalDevice device, PhysicalDeviceInfo&) const
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensions(m_renderSettings.m_deviceExtensions.begin(), m_renderSettings.m_deviceExtensions.end());

    for (VkExtensionProperties const& extension : availableExtensions)
    {
        requiredExtensions.erase(extension.extensionName);
    }

    return requiredExtensions.empty();
}

bool Renderer::CheckSwapchainSupport(VkPhysicalDevice device, PhysicalDeviceInfo& deviceInfo) const
{
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_surface, &deviceInfo.m_capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, nullptr);

	if (formatCount != 0)
	{
		deviceInfo.m_surfaceFormats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_surface, &formatCount, deviceInfo.m_surfaceFormats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, nullptr);

	if (presentModeCount != 0)
	{
		deviceInfo.m_presentModes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_surface, &presentModeCount, deviceInfo.m_presentModes.data());
	}

    bool const swapchainSupported = (deviceInfo.m_capabilities.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        && !deviceInfo.m_surfaceFormats.empty() 
        && !deviceInfo.m_presentModes.empty();

	return swapchainSupported;
}

bool Renderer::CheckPhysicalDeviceProperties(VkPhysicalDevice device, PhysicalDeviceInfo& deviceInfo) const
{
    deviceInfo.m_dynamicRenderingFeature = {};
    deviceInfo.m_dynamicRenderingFeature.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DYNAMIC_RENDERING_FEATURES;

    deviceInfo.m_features = {};
    deviceInfo.m_features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2;
    PNextChainPushBack(&deviceInfo.m_features, &deviceInfo.m_dynamicRenderingFeature);
    
    vkGetPhysicalDeviceFeatures2(device, &deviceInfo.m_features);
    vkGetPhysicalDeviceProperties(device, &deviceInfo.m_properties);
    vkGetPhysicalDeviceMemoryProperties(device, &deviceInfo.m_memoryProperties);

    bool const anisotropyCheck = !m_renderSettings.m_useAnisotropy || deviceInfo.m_features.features.samplerAnisotropy;

    return anisotropyCheck
        && deviceInfo.m_dynamicRenderingFeature.dynamicRendering;
} 

void Renderer::SelectBestPhysicalDevice(VkPhysicalDevice device, PhysicalDeviceInfo const& deviceInfo)
{
    if (m_physicalDevice == VK_NULL_HANDLE || m_physicalDevice == device)
    {
        m_physicalDevice = device;
        m_physicalDeviceInfo = deviceInfo;
        return;
    }

    // Device type
    VkPhysicalDeviceType const currentDeviceType = m_physicalDeviceInfo.m_properties.deviceType;
    VkPhysicalDeviceType const candidateDeviceType = deviceInfo.m_properties.deviceType;
    if (currentDeviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        && candidateDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        m_physicalDevice = device;
        m_physicalDeviceInfo = deviceInfo;
        return;
    }
    else if (currentDeviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU
        && candidateDeviceType != VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
    {
        return;
    }

    // VRAM
    auto getVRAM = [](PhysicalDeviceInfo const& info) -> uint64_t
    {
        uint64_t totalSize = 0;
        for (uint32_t i = 0; i < info.m_memoryProperties.memoryHeapCount; ++i)
        {
            VkMemoryHeap const& heap = info.m_memoryProperties.memoryHeaps[i];
            if (heap.flags & VkMemoryHeapFlagBits::VK_MEMORY_HEAP_DEVICE_LOCAL_BIT)
            {
                totalSize += heap.size;
            }
        }
        return totalSize;
    };
    uint64_t const currentDeviceVRAM = getVRAM(m_physicalDeviceInfo);
    uint64_t const candidateDeviceVRAM = getVRAM(deviceInfo);
    if (candidateDeviceVRAM > currentDeviceVRAM)
    {
        m_physicalDevice = device;
        m_physicalDeviceInfo = deviceInfo;
        return;
    }
}

VkSurfaceFormatKHR Renderer::ChooseSwapchainSurfaceFormat() const
{
    for (VkSurfaceFormatKHR const& availableFormat : m_physicalDeviceInfo.m_surfaceFormats)
    {
        if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM && availableFormat.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
        {
            return availableFormat;
        }
    }

    return m_physicalDeviceInfo.m_surfaceFormats.front();
}

VkPresentModeKHR Renderer::ChooseSwapchainPresentMode() const
{
    for (VkPresentModeKHR const& availablePresentMode : m_physicalDeviceInfo.m_presentModes)
    {
        if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR && !m_renderSettings.m_useVSync
            || availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR && m_renderSettings.m_useVSync)
        {
            return availablePresentMode;
        }
    }

    return VK_PRESENT_MODE_FIFO_KHR;
}

VkExtent2D Renderer::ChooseSwapchainExtent() const
{
    if (m_physicalDeviceInfo.m_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
    {
        return m_physicalDeviceInfo.m_capabilities.currentExtent;
    }

    int32_t width, height;
    glfwGetFramebufferSize(m_window, &width, &height);

    VkExtent2D actualExtent = {
        static_cast<uint32_t>(width),
        static_cast<uint32_t>(height)
    };

    actualExtent.width = std::clamp(actualExtent.width, m_physicalDeviceInfo.m_capabilities.minImageExtent.width, m_physicalDeviceInfo.m_capabilities.maxImageExtent.width);
    actualExtent.height = std::clamp(actualExtent.height, m_physicalDeviceInfo.m_capabilities.minImageExtent.height, m_physicalDeviceInfo.m_capabilities.maxImageExtent.height);

    return actualExtent;
}

/*static*/ VkImageAspectFlags Renderer::GetAspectFlagsFromFormat(VkFormat format)
{
    if (Contains(s_sortedDepthFormats, format))
    {
        if (Contains(s_sortedDepthStencilFormats, format))
        {
            return VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
        }

        return VK_IMAGE_ASPECT_DEPTH_BIT;
    }

    return VK_IMAGE_ASPECT_COLOR_BIT;
}

VkFormat Renderer::ChooseDepthFormat(bool requireStencil /*= false*/) const
{
    return FindSupportedFormat(
        requireStencil ? s_sortedDepthStencilFormats : s_sortedDepthFormats,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
    );
}

VkFormat Renderer::ChooseBackbufferFormat() const
{
    return FindSupportedFormat(
        s_sortedBackbufferFormats,
        VK_IMAGE_TILING_OPTIMAL,
        VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT | VK_FORMAT_FEATURE_BLIT_SRC_BIT
    );
}

/*static*/ bool Renderer::FormatHasStencil(VkFormat format)
{
    return Contains(s_sortedDepthStencilFormats, format);
}

VkFormat Renderer::FindSupportedFormat(std::vector<VkFormat> const& sortedCandidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
    for (VkFormat const& format : sortedCandidates)
    {
        VkFormatProperties formatProperties;
        vkGetPhysicalDeviceFormatProperties(m_physicalDevice, format, &formatProperties);

        if (tiling == VK_IMAGE_TILING_LINEAR && 
            (formatProperties.linearTilingFeatures & features) == features)
        {
            return format;
        }
        else if (tiling == VK_IMAGE_TILING_OPTIMAL &&
            (formatProperties.optimalTilingFeatures & features) == features)
        {
            return format;
        }
    }

    ThrowError("Failed to find a supported format");
    return VK_FORMAT_UNDEFINED;
}

VkCommandBuffer Renderer::BeginSingleUseCommandBuffer()
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_singleUseCommandPool;
    allocInfo.commandBufferCount = 1;

    VkCommandBuffer commandBuffer;
    vkAllocateCommandBuffers(m_device, &allocInfo, &commandBuffer);

    VkCommandBufferBeginInfo beginInfo = {};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

    vkBeginCommandBuffer(commandBuffer, &beginInfo);

    return commandBuffer;
}

void Renderer::EndSingleUseCommandBuffer(VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkFence commandsCompletedFence;
    VkFenceCreateInfo fenceInfo = {};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    vkCreateFence(m_device, &fenceInfo, nullptr, &commandsCompletedFence);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    vkQueueSubmit(m_graphicsQueue, 1, &submitInfo, commandsCompletedFence);
    vkWaitForFences(m_device, 1, &commandsCompletedFence, true, UINT64_MAX);

    vkDestroyFence(m_device, commandsCompletedFence, VK_NULL_HANDLE);
    vkFreeCommandBuffers(m_device, m_singleUseCommandPool, 1, &commandBuffer);
}

void Renderer::PopulateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& debugMessengerInfo) const
{
    debugMessengerInfo = {};
    debugMessengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    debugMessengerInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
    debugMessengerInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    debugMessengerInfo.pfnUserCallback = DebugCallback;
}

/*static*/ VKAPI_ATTR VkBool32 VKAPI_CALL Renderer::DebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT, VkDebugUtilsMessageTypeFlagsEXT, VkDebugUtilsMessengerCallbackDataEXT const* callbackData, void*)
{
    Log("[Validation layer] %s", callbackData->pMessage);

    return false;
}

/*static*/ void Renderer::FramebufferResizeCallback(GLFWwindow*, int, int)
{
    Renderer::GetInstance().m_framebufferResized = true;
}

/*static*/ void Renderer::GetAccessMasksForLayoutTransition(VkImageLayout oldLayout, VkImageLayout newLayout, VkAccessFlags& srcAccessMask, VkAccessFlags& dstAccessMask)
{
    // Source layout defines the source access mask which controls actions that have to be finished on the old layout before it will be transitioned to the new layout
    switch (oldLayout)
    {
    case VK_IMAGE_LAYOUT_UNDEFINED:
        // Image layout is undefined (or does not matter)
        // Only valid as initial layout
        // No flags required, listed only for completeness
        srcAccessMask = 0;
        break;

    case VK_IMAGE_LAYOUT_PREINITIALIZED:
        // Image is preinitialized
        // Only valid as initial layout for linear images, preserves memory contents
        // Make sure host writes have been finished
        srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        // Image is a color attachment
        // Make sure any writes to the color buffer have been finished
        srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        // Image is a depth/stencil attachment
        // Make sure any writes to the depth/stencil buffer have been finished
        srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Image is a transfer source
        // Make sure any reads from the image have been finished
        srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Image is a transfer destination
        // Make sure any writes to the image have been finished
        srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Image is read by a shader
        // Make sure any shader reads from the image have been finished
        srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;

    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        // Synchronization regarding presentation
        // is handled in the by our fences and semaphores
        srcAccessMask = 0;
        break;
        
    default:
        srcAccessMask = 0;
        Warn("Unsupported source layout: %d.", oldLayout);
        break;
    }

    // Target layout defines the destination access mask which controls the dependency for the new image layout
    switch (newLayout)
    {
    case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
        // Image will be used as a transfer destination
        // Make sure any writes to the image have been finished
        dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
        // Image will be used as a transfer source
        // Make sure any reads from the image have been finished
        dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
        break;

    case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
        // Image will be used as a color attachment
        // Make sure any writes to the color buffer have been finished
        dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
        // Image layout will be used as a depth/stencil attachment
        // Make sure any writes to depth/stencil buffer have been finished
        dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
        break;

    case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
        // Image will be read in a shader (sampler, input attachment)
        // Make sure any writes to the image have been finished
        if (srcAccessMask == 0)
        {
            srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
        }
        dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
        break;

    case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
        // There is no need to delay subsequent processing, or perform any visibility operations
        // (as vkQueuePresentKHR performs automatic visibility operations).
        dstAccessMask = 0;
        break;

    default:
        dstAccessMask = 0;
        Warn("Unsupported target layout: %d.", newLayout);
        break;
    }
}

VkSampleCountFlagBits Renderer::GetRasterizationSampleCount() const
{
    if (!m_renderSettings.m_useMultisampling)
    {
        return VK_SAMPLE_COUNT_1_BIT;
    }

    if (m_renderSettings.m_rasterizationSampleCount != VK_SAMPLE_COUNT_FLAG_BITS_MAX_ENUM
        && (m_renderSettings.m_rasterizationSampleCount & m_physicalDeviceInfo.m_properties.limits.framebufferColorSampleCounts))
    {
        return m_renderSettings.m_rasterizationSampleCount;
    }

    static const std::array<VkSampleCountFlagBits, 7> desiredBits = { VK_SAMPLE_COUNT_64_BIT, VK_SAMPLE_COUNT_32_BIT, VK_SAMPLE_COUNT_16_BIT, VK_SAMPLE_COUNT_8_BIT, VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_2_BIT, VK_SAMPLE_COUNT_1_BIT };
    for (VkSampleCountFlagBits const& bit : desiredBits)
    {
        if (bit & m_physicalDeviceInfo.m_properties.limits.framebufferColorSampleCounts)
        {
			return bit;
        }
    }

    return VK_SAMPLE_COUNT_1_BIT;
}
