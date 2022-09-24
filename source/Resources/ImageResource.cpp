#include <Resources/ImageResource.hpp>

#include <Systems/Renderer.hpp>
#include <Utilities/Helpers.hpp>

void ImageResource::CreateImage()
{
    VkExtent2D const extent = GetExtent();

    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.extent.width = extent.width;
    imageInfo.extent.height = extent.height;
    imageInfo.extent.depth = 1;
    imageInfo.mipLevels = m_creationInfo.m_mipLevels;
    imageInfo.arrayLayers = m_creationInfo.m_layers;
    imageInfo.format = m_creationInfo.m_format;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = m_imageUsage;
    imageInfo.samples = m_creationInfo.m_sampleCount;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.flags = (m_creationInfo.m_layers == 6) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0;

    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

    if (vmaCreateImage(Renderer::GetInstance().GetAllocator(), &imageInfo, &allocationInfo, &m_image, &m_deviceMemory, nullptr) != VK_SUCCESS)
    {
        ThrowError("Failed to create image.");
    }

    m_needsRecreation = false;
}

void ImageResource::CreateImageView()
{
    VkImageAspectFlags const aspectFlags = Renderer::GetAspectFlagsFromFormat(m_creationInfo.m_format);

    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.image = m_image;
    imageViewInfo.viewType = (m_creationInfo.m_layers == 6) ? VK_IMAGE_VIEW_TYPE_CUBE : VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = m_creationInfo.m_format;
    imageViewInfo.subresourceRange.aspectMask = aspectFlags;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.levelCount = m_creationInfo.m_mipLevels;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.layerCount = m_creationInfo.m_layers;

    if (vkCreateImageView(Renderer::GetInstance().GetDevice(), &imageViewInfo, nullptr, &m_imageView) != VK_SUCCESS)
    {
        ThrowError("Failed to create texture image view.");
    }
}

void ImageResource::Destroy()
{
    Renderer& renderer = Renderer::GetInstance();
    if (m_imageView != VK_NULL_HANDLE)
    {
        vkDestroyImageView(renderer.GetDevice(), m_imageView, nullptr);
    }

    if (m_image != VK_NULL_HANDLE && m_deviceMemory != VK_NULL_HANDLE)
    {
        vmaDestroyImage(renderer.GetAllocator(), m_image, m_deviceMemory);
    }

    m_image = VK_NULL_HANDLE;
    m_deviceMemory = VK_NULL_HANDLE;
    m_imageView = VK_NULL_HANDLE;

    m_imageUsage = 0;
    m_needsRecreation = false;
    m_currentLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    m_creationInfo = ImageCreateInfo();
}

ImageResource::~ImageResource()
{
    Destroy();
}

ImageResource::ImageResource(ImageResource&& other) noexcept
{
    m_image = other.m_image;
    m_deviceMemory = other.m_deviceMemory; 
    m_imageView = other.m_imageView;

    other.m_image = VK_NULL_HANDLE;
    other.m_deviceMemory = VK_NULL_HANDLE;
    other.m_imageView = VK_NULL_HANDLE;

    m_imageUsage = other.m_imageUsage;
    m_needsRecreation = other.m_needsRecreation;
    m_currentLayout = other.m_currentLayout;
    m_creationInfo = std::move(other.m_creationInfo);
}

ImageResource& ImageResource::operator=(ImageResource&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    Destroy();

    m_image = other.m_image;
    m_deviceMemory = other.m_deviceMemory;
    m_imageView = other.m_imageView; 

    other.m_image = VK_NULL_HANDLE;
    other.m_deviceMemory = VK_NULL_HANDLE;
    other.m_imageView = VK_NULL_HANDLE;

    m_imageUsage = other.m_imageUsage;
    m_needsRecreation = other.m_needsRecreation;
    m_currentLayout = other.m_currentLayout;
    m_creationInfo = std::move(other.m_creationInfo);

    return *this;
}

void ImageResource::AddImageUsageFlags(VkImageUsageFlags flags)
{
    if ((m_imageUsage & flags) == 0)
    {
        m_imageUsage |= flags;
        m_needsRecreation = true;
    }
}

VkExtent2D ImageResource::GetExtent() const
{
    return GetExtent(m_creationInfo);
}

/*static*/ VkExtent2D ImageResource::GetExtent(ImageCreateInfo const& creationInfo)
{
    if (creationInfo.m_sizeType == SizeType::Absolute)
    {
        return VkExtent2D{ (uint16_t)creationInfo.m_width, (uint16_t)creationInfo.m_height };
    }
    else if (creationInfo.m_sizeType == SizeType::RelativeToSwapchain)
    {
        Renderer const& renderer = Renderer::GetInstance();
        VkExtent2D const& swapchainExtent = renderer.GetSwapchainExtent();
        return VkExtent2D{ (uint16_t)(creationInfo.m_width * swapchainExtent.width), (uint16_t)(creationInfo.m_height * swapchainExtent.height) };
    }

    Warn("Invalid attachment size type: %d", creationInfo.m_sizeType);
    return VkExtent2D{ (uint16_t)creationInfo.m_width, (uint16_t)creationInfo.m_height };
}

bool ImageResource::IsValid() const
{
    return m_image != VK_NULL_HANDLE &&
        m_imageView != VK_NULL_HANDLE &&
        m_deviceMemory != VK_NULL_HANDLE;
}

void ImageResource::TransitionLayout(VkImageLayout newLayout, VkPipelineStageFlags srcStageMask, VkPipelineStageFlags dstStageMask, VkCommandBuffer commandBuffer)
{
    VkImageMemoryBarrier imageMemoryBarrier = {};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.image = m_image;
    imageMemoryBarrier.oldLayout = m_currentLayout;
    imageMemoryBarrier.newLayout = newLayout;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

    imageMemoryBarrier.subresourceRange.aspectMask = Renderer::GetAspectFlagsFromFormat(m_creationInfo.m_format);
    imageMemoryBarrier.subresourceRange.baseMipLevel = 0;
    imageMemoryBarrier.subresourceRange.levelCount = m_creationInfo.m_mipLevels;
    imageMemoryBarrier.subresourceRange.baseArrayLayer = 0;
    imageMemoryBarrier.subresourceRange.layerCount = m_creationInfo.m_layers;

    Renderer::GetAccessMasksForLayoutTransition(m_currentLayout, newLayout, imageMemoryBarrier.srcAccessMask, imageMemoryBarrier.dstAccessMask);

    vkCmdPipelineBarrier(
        commandBuffer,
        srcStageMask,
        dstStageMask,
        0,
        0, nullptr,
        0, nullptr,
        1, &imageMemoryBarrier);

    m_currentLayout = newLayout;
}

void ImageResource::GenerateMipmaps(VkCommandBuffer commandBuffer, VkImageLayout finalLayout)
{
    VkImageAspectFlags const aspectFlags = Renderer::GetAspectFlagsFromFormat(m_creationInfo.m_format);
    VkExtent2D const extent = GetExtent();

    for (uint8_t layer = 0; layer < m_creationInfo.m_layers; layer++)
    {
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = m_image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = aspectFlags;
        barrier.subresourceRange.baseArrayLayer = layer;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mipWidth = extent.width;
        int32_t mipHeight = extent.height;

        for (uint32_t mipLevel = 1; mipLevel < m_creationInfo.m_mipLevels; mipLevel++)
        {
            barrier.subresourceRange.baseMipLevel = mipLevel - 1;
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            Renderer::GetAccessMasksForLayoutTransition(barrier.oldLayout, barrier.newLayout, barrier.srcAccessMask, barrier.dstAccessMask);

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            VkImageBlit blit = {};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = {mipWidth, mipHeight, 1};
            blit.srcSubresource.aspectMask = aspectFlags;
            blit.srcSubresource.mipLevel = mipLevel - 1;
            blit.srcSubresource.baseArrayLayer = layer;
            blit.srcSubresource.layerCount = 1;
            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = { mipWidth > 1 ? mipWidth / 2 : 1, mipHeight > 1 ? mipHeight / 2 : 1, 1 };
            blit.dstSubresource.aspectMask = aspectFlags;
            blit.dstSubresource.mipLevel = mipLevel;
            blit.dstSubresource.baseArrayLayer = layer;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(commandBuffer,
                m_image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                m_image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                1, &blit,
                VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = finalLayout;
            Renderer::GetAccessMasksForLayoutTransition(barrier.oldLayout, barrier.newLayout, barrier.srcAccessMask, barrier.dstAccessMask);

            vkCmdPipelineBarrier(commandBuffer,
                VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0,
                0, nullptr,
                0, nullptr,
                1, &barrier);

            mipWidth  = (mipWidth  > 1) ? (mipWidth  /= 2) : mipWidth;
            mipHeight = (mipHeight > 1) ? (mipHeight /= 2) : mipHeight;
        }

        barrier.subresourceRange.baseMipLevel = m_creationInfo.m_mipLevels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = finalLayout;
        Renderer::GetAccessMasksForLayoutTransition(barrier.oldLayout, barrier.newLayout, barrier.srcAccessMask, barrier.dstAccessMask);

        vkCmdPipelineBarrier(commandBuffer,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_ALL_GRAPHICS_BIT, 0,
            0, nullptr,
            0, nullptr,
            1, &barrier);
    }

    m_currentLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
}
