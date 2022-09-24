#pragma once

#include <Resources/ImageResource.hpp>
#include <Utilities/Helpers.hpp>

class Buffer;

struct TextureSampler
{
    VkFilter m_magFilter = VK_FILTER_LINEAR;
    VkFilter m_minFilter = VK_FILTER_LINEAR;
    VkSamplerAddressMode m_addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode m_addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode m_addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    float m_anisotropy = FLT_MAX;
};

struct TextureCreationInfo
{
    ImageCreateInfo m_imageCreateInfo;
    uint8_t m_channels = 0;
    uint8_t m_bytesPerChannel = 0;
    TextureSampler m_samplerInfo;
    void const* m_data = nullptr;
};

class TextureResource 
    : public std::enable_shared_from_this<TextureResource>
{
public:
    TextureResource() = default;
    TextureResource(TextureCreationInfo const& creationInfo);
    ~TextureResource();

    TextureResource(TextureResource&&) noexcept;
    TextureResource& operator=(TextureResource&&) noexcept;
    TextureResource(TextureResource const&) = delete;
    TextureResource& operator=(TextureResource const&) = delete;

    inline SharedPtr<TextureResource> GetSharedPtr() { return shared_from_this(); }

    void CreateFromImage(SharedPtr<ImageResource>& image);
    void Destroy();

    inline TextureCreationInfo const& GetCreationInfo() const { return m_creationInfo; }
    inline ImageCreateInfo const& GetImageCreationInfo() const { return m_creationInfo.m_imageCreateInfo; }
    inline void SetCreationInfo(TextureCreationInfo const& creationInfo) { m_creationInfo = creationInfo; }
    inline ImageResource const& GetImage() const { return *m_image; }
    inline ImageResource& GetImage() { return *m_image; }
    inline void SetReadInPass(uint64_t passId) { m_readInPasses.insert(passId); }
    inline void SetIsPersistent(bool persistent) { m_isPersistent = persistent; }
    inline bool IsUsed() { return !m_readInPasses.empty(); }
    inline bool IsPersistent() { return m_isPersistent; }
    bool IsValid() const;
    VkDescriptorImageInfo GetDescriptorInfo() const;

    void CopyFromBuffer(VkCommandBuffer commandBuffer, Buffer const& buffer);

private:
    void CreateImage();
    void CreateImageWithData();
    void CreateImageWithoutData();
    void CreateTextureSampler();
    

private:
    SharedPtr<ImageResource> m_image = nullptr;

    VkSampler m_sampler = VK_NULL_HANDLE;

    TextureCreationInfo m_creationInfo;
    std::set<uint64_t> m_readInPasses;
    bool m_isPersistent = false;
};
