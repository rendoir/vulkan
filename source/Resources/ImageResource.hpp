#pragma once

enum class SizeType
{
    Absolute,
    RelativeToSwapchain
};

struct ImageCreateInfo
{
    float m_width = 1.0f;
    float m_height = 1.0f;
    SizeType m_sizeType = SizeType::RelativeToSwapchain;
    uint8_t m_layers = 1;
    uint8_t m_mipLevels = 1;
    VkFormat m_format = VK_FORMAT_UNDEFINED;
    VkSampleCountFlagBits m_sampleCount = VK_SAMPLE_COUNT_1_BIT;
};

class ImageResource
{
public:
    ImageResource() = default;
    ~ImageResource();

    ImageResource(ImageResource&&) noexcept;
    ImageResource& operator=(ImageResource&&) noexcept;
    ImageResource(ImageResource const&) = delete;
    ImageResource& operator=(ImageResource const&) = delete;

    void CreateImage();
    void CreateImageView();
    void Destroy();

    inline VkImage GetImage() const { return m_image; }
    inline VkImageView GetImageView() const { return m_imageView; }
    inline VkImageLayout GetCurrentLayout() const { return m_currentLayout; }
    inline VkImageUsageFlags GetImageUsage() const { return m_imageUsage; }
    inline bool NeedsRecreation() const { return m_needsRecreation; }
    inline void SetNeedsRecreation() { m_needsRecreation = true; }
    inline ImageCreateInfo GetCreationInfo() const { return m_creationInfo; }
    inline void SetCreationInfo(ImageCreateInfo const& creationInfo) { m_creationInfo = creationInfo; }
    inline void SetImage(VkImage image) { m_image = image; }
    
    bool IsValid() const;
    VkExtent2D GetExtent() const;
    static VkExtent2D GetExtent(ImageCreateInfo const& creationInfo);
    void AddImageUsageFlags(VkImageUsageFlags flags);
    void TransitionLayout(VkImageLayout newLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkCommandBuffer commandBuffer);
    void GenerateMipmaps(VkCommandBuffer commandBuffer, VkImageLayout finalLayout);

private:
    VkImage m_image = VK_NULL_HANDLE;
    VkImageView m_imageView = VK_NULL_HANDLE;
    VmaAllocation m_deviceMemory = VK_NULL_HANDLE;

    VkImageUsageFlags m_imageUsage = 0;
    VkImageLayout m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    ImageCreateInfo m_creationInfo;

    bool m_needsRecreation = false;

    friend class AttachmentResource;
    friend class TextureResource;
};
