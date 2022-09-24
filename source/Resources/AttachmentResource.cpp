#include <Resources/AttachmentResource.hpp>

AttachmentResource::AttachmentResource()
{
    m_image = std::make_shared<ImageResource>();
}

void AttachmentResource::Create()
{
    if (IsValid())
    {
        VkImageUsageFlags const oldImageUsage = m_image->GetImageUsage();
        m_image = std::make_shared<ImageResource>();
        m_image->AddImageUsageFlags(oldImageUsage);
    }

    ImageCreateInfo const& imageCreateInfo = m_creationInfo.m_imageCreateInfo;
    m_image->SetCreationInfo(imageCreateInfo);
    m_image->CreateImage();
    m_image->CreateImageView();
}

void AttachmentResource::Destroy()
{
    m_image = std::make_shared<ImageResource>();
    m_readInPasses.clear();
    m_writtenInPasses.clear();
    m_clearValue.reset();
    m_creationInfo = AttachmentCreationInfo();
}

AttachmentResource::~AttachmentResource()
{
    Destroy();
}

AttachmentResource::AttachmentResource(AttachmentResource&& other) noexcept
{
    m_image = other.m_image;
    m_creationInfo = std::move(other.m_creationInfo);
    m_readInPasses = std::move(other.m_readInPasses);
    m_writtenInPasses = std::move(other.m_writtenInPasses);
    m_clearValue = other.m_clearValue;

    other.m_image = nullptr;
}

AttachmentResource& AttachmentResource::operator=(AttachmentResource&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    Destroy();
    
    m_image = other.m_image;
    m_creationInfo = std::move(other.m_creationInfo);
    m_readInPasses = std::move(other.m_readInPasses);
    m_writtenInPasses = std::move(other.m_writtenInPasses);
    m_clearValue = other.m_clearValue;

    other.m_image = nullptr;

    return *this;
}

void AttachmentResource::SetCreationInfo(AttachmentCreationInfo const& creationInfo)
{
    m_creationInfo = creationInfo;
    
    if (m_creationInfo.m_imageCreateInfo.m_mipLevels == UINT8_MAX) 
    {
        // Generate the maximum amount of mip levels
        VkExtent2D const extent = ImageResource::GetExtent(m_creationInfo.m_imageCreateInfo);
        m_creationInfo.m_imageCreateInfo.m_mipLevels = floor(log2(std::max(extent.width, extent.height))) + 1;
    }
}

bool AttachmentResource::IsValid() const
{
    return m_image && m_image->IsValid();
}

bool AttachmentResource::IsReadInPass(uint64_t passId) const
{
    return Contains(m_readInPasses, passId);
}

bool AttachmentResource::IsWrittenInPass(uint64_t passId) const
{
    return Contains(m_writtenInPasses, passId);
}

void AttachmentResource::RemovePassUsage(uint64_t passId)
{
    auto readIt = std::find(m_readInPasses.begin(), m_readInPasses.end(), passId);
    if (readIt != m_readInPasses.end())
    {
        m_readInPasses.erase(readIt);
    }

    auto writeIt = std::find(m_writtenInPasses.begin(), m_writtenInPasses.end(), passId);
    if (writeIt != m_writtenInPasses.end())
    {
        m_writtenInPasses.erase(writeIt);
    }
}

VkClearValue AttachmentResource::GetClearValue() const
{
    if (m_clearValue.has_value())
    {
        return m_clearValue.value();
    }

    VkClearValue defaultClearValue = {};
    VkImageUsageFlags const imageUsage = m_image->GetImageUsage();
    if (imageUsage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
    {
        defaultClearValue.color = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    }
    else if (imageUsage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
    {
        defaultClearValue.depthStencil = { 1.0f, 0 };
    }

    return defaultClearValue;
}
