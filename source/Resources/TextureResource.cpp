#include <Resources/TextureResource.hpp>

#include <Resources/Buffer.hpp>
#include <Systems/Renderer.hpp>

TextureResource::TextureResource(TextureCreationInfo const& creationInfo)
{
    m_creationInfo = creationInfo;
    m_image = std::make_shared<ImageResource>();

    if (m_creationInfo.m_imageCreateInfo.m_mipLevels == UINT8_MAX) 
    {
        // Generate the maximum amount of mip levels
        VkExtent2D const extent = ImageResource::GetExtent(m_creationInfo.m_imageCreateInfo);
        m_creationInfo.m_imageCreateInfo.m_mipLevels = floor(log2(std::max(extent.width, extent.height))) + 1;
    }

    ImageCreateInfo const& imageCreateInfo = m_creationInfo.m_imageCreateInfo;
    m_image->SetCreationInfo(imageCreateInfo);

    CreateImage();

    m_image->CreateImageView();
    
    CreateTextureSampler();
}

void TextureResource::CreateFromImage(SharedPtr<ImageResource>& image)
{
    m_image = image;

    CreateTextureSampler();
}

void TextureResource::Destroy()
{
    m_image = std::make_shared<ImageResource>();
    m_creationInfo = TextureCreationInfo();
    m_readInPasses.clear();
    m_isPersistent = false;

    if (m_sampler != VK_NULL_HANDLE)
    {
        Renderer& renderer = Renderer::GetInstance();
        vkDestroySampler(renderer.GetDevice(), m_sampler, nullptr);
    }

    m_sampler = VK_NULL_HANDLE;
}

TextureResource::~TextureResource()
{
    Destroy();
}

TextureResource::TextureResource(TextureResource&& other) noexcept
{
    m_image = other.m_image;
    m_creationInfo = std::move(other.m_creationInfo);
    m_sampler = other.m_sampler;
    m_readInPasses = std::move(other.m_readInPasses);
    m_isPersistent = other.m_isPersistent;

    other.m_image = nullptr;
    other.m_sampler = VK_NULL_HANDLE;
}

TextureResource& TextureResource::operator=(TextureResource&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    Destroy();
    
    m_image = other.m_image;
    m_creationInfo = std::move(other.m_creationInfo);
    m_sampler = other.m_sampler;
    m_readInPasses = std::move(other.m_readInPasses);
    m_isPersistent = other.m_isPersistent;

    other.m_image = nullptr;
    other.m_sampler = VK_NULL_HANDLE;

    return *this;
}

void TextureResource::CreateImage()
{
    if (m_creationInfo.m_data != nullptr)
    {
        CreateImageWithData();
    }
    else
    {
        CreateImageWithoutData();
    }
}

void TextureResource::CreateImageWithData()
{
    Renderer& renderer = Renderer::GetInstance();
    ImageCreateInfo& imageCreateInfo = m_creationInfo.m_imageCreateInfo;
    VkExtent2D const extent = m_image->GetExtent();

    uint64_t const imageSize = extent.width * extent.height * m_creationInfo.m_channels * m_creationInfo.m_bytesPerChannel * imageCreateInfo.m_layers;

    BufferInfo bufferInfo;
    bufferInfo.m_size = imageSize;
    bufferInfo.m_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.m_memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

    Buffer stagingBuffer = Buffer(bufferInfo);
    stagingBuffer.CopyDataToBuffer(m_creationInfo.m_data, imageSize);

    m_image->AddImageUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    m_image->CreateImage();

    VkCommandBuffer commandBuffer = renderer.BeginSingleUseCommandBuffer();
    m_image->TransitionLayout(VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, commandBuffer);
    CopyFromBuffer(commandBuffer, stagingBuffer);
    m_image->GenerateMipmaps(commandBuffer, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
    renderer.EndSingleUseCommandBuffer(commandBuffer);
}

void TextureResource::CreateImageWithoutData()
{
    Renderer& renderer = Renderer::GetInstance();

    m_image->AddImageUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT);
    m_image->CreateImage();

    VkCommandBuffer commandBuffer = renderer.BeginSingleUseCommandBuffer();
    m_image->TransitionLayout(VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, commandBuffer);
    renderer.EndSingleUseCommandBuffer(commandBuffer);
}

void TextureResource::CreateTextureSampler()
{
    Renderer& renderer = Renderer::GetInstance();
    Renderer::RenderSettings const& renderSettings = renderer.GetRenderSettings();
    Renderer::PhysicalDeviceInfo const& physicalDeviceInfo = renderer.GetPhysicalDeviceInfo();
    
    m_creationInfo.m_samplerInfo.m_anisotropy = std::min(m_creationInfo.m_samplerInfo.m_anisotropy, physicalDeviceInfo.m_properties.limits.maxSamplerAnisotropy);

    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = m_creationInfo.m_samplerInfo.m_magFilter;
    samplerInfo.minFilter = m_creationInfo.m_samplerInfo.m_minFilter;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = m_creationInfo.m_samplerInfo.m_addressModeU;
    samplerInfo.addressModeV = m_creationInfo.m_samplerInfo.m_addressModeV;
    samplerInfo.addressModeW = m_creationInfo.m_samplerInfo.m_addressModeW;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.maxAnisotropy = renderSettings.m_useAnisotropy ? m_creationInfo.m_samplerInfo.m_anisotropy : 1.0f;
    samplerInfo.anisotropyEnable = renderSettings.m_useAnisotropy && samplerInfo.maxAnisotropy > 1.0f;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = static_cast<float>(m_creationInfo.m_imageCreateInfo.m_mipLevels);
    samplerInfo.mipLodBias = 0;

    if (vkCreateSampler(renderer.GetDevice(), &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
    {
        ThrowError("Failed to create texture sampler.");
    }
}

void TextureResource::CopyFromBuffer(VkCommandBuffer commandBuffer, Buffer const& buffer)
{
    VkImageAspectFlags const aspectFlags = Renderer::GetAspectFlagsFromFormat(m_creationInfo.m_imageCreateInfo.m_format);
    VkExtent2D const extent = m_image->GetExtent();
    uint64_t const layerSize = extent.width * extent.height * m_creationInfo.m_channels * m_creationInfo.m_bytesPerChannel;

    std::vector<VkBufferImageCopy> bufferCopyRegions;
    bufferCopyRegions.reserve(m_creationInfo.m_imageCreateInfo.m_layers);
    
    uint64_t offset = 0;
    for (uint32_t layer = 0; layer < m_creationInfo.m_imageCreateInfo.m_layers; layer++)
    {
        VkBufferImageCopy bufferCopyRegion = {};
        bufferCopyRegion.bufferOffset = offset;
        bufferCopyRegion.bufferRowLength = 0;
        bufferCopyRegion.bufferImageHeight = 0;
        bufferCopyRegion.imageSubresource.aspectMask = aspectFlags;
        bufferCopyRegion.imageSubresource.mipLevel = 0;
        bufferCopyRegion.imageSubresource.baseArrayLayer = layer;
        bufferCopyRegion.imageSubresource.layerCount = 1;
        bufferCopyRegion.imageOffset = {0, 0, 0};
        bufferCopyRegion.imageExtent = {extent.width, extent.height, 1};

        bufferCopyRegions.push_back(bufferCopyRegion);

        offset += layerSize;
    }

    vkCmdCopyBufferToImage(commandBuffer, buffer.GetBuffer(), m_image->GetImage(), m_image->GetCurrentLayout(), bufferCopyRegions.size(), bufferCopyRegions.data());
}

bool TextureResource::IsValid() const
{
    return m_image && m_image->IsValid()
        && m_sampler != VK_NULL_HANDLE;
}

VkDescriptorImageInfo TextureResource::GetDescriptorInfo() const
{
    VkDescriptorImageInfo descriptorInfo;
    descriptorInfo.imageView = m_image->m_imageView;
    descriptorInfo.imageLayout = m_image->m_currentLayout;
    descriptorInfo.sampler = m_sampler;
    return descriptorInfo;
}
