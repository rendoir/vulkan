#include <Systems/RenderGraph.hpp>

#include <Resources/ImageResource.hpp>
#include <Systems/Renderer.hpp>

void RenderGraph::Init()
{
    CreateCommandPool();
    CreateCommandBuffers();
    Renderer::GetInstance().AddSwapchainObserver(this);
}

void RenderGraph::Terminate()
{
    Renderer::GetInstance().RemoveSwapchainObserver(this);
    TerminateRenderPasses();
    DestroyAttachments();
    DestroyTextures();
    DestroyCommandPool();
}

void RenderGraph::BeginCommandBuffer(VkCommandBuffer commandBuffer)
{
    VkCommandBufferBeginInfo beginCommandBufferInfo = {};
    beginCommandBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

    if (vkBeginCommandBuffer(commandBuffer, &beginCommandBufferInfo) != VK_SUCCESS) 
    {
        ThrowError("Failed to begin recording command buffer.");
    }
}

void RenderGraph::EndCommandBuffer(VkCommandBuffer commandBuffer)
{
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS)
    {
        ThrowError("Failed to record command buffer.");
    }
}

void RenderGraph::Execute(VkCommandBuffer commandBuffer)
{
    BeginCommandBuffer(commandBuffer);

    UpdateAttachments();
    UpdateTextures();
    DestroyRenderPassesPendingRemoval();
    ExecuteInternal(commandBuffer);

    EndCommandBuffer(commandBuffer);
}

void RenderGraph::ExecuteInternal(VkCommandBuffer commandBuffer)
{
    ExecutionContext context;
    context.m_uniqueColorAttachments.reserve(m_attachments.size());
    context.m_uniqueDepthStencilAttachments.reserve(m_attachments.size());

    for (UniquePtr<RenderPass>& pass : m_renderPasses)
    {
        pass->Execute(commandBuffer, context);
    }
}

void RenderGraph::UpdateAttachments()
{
    auto it = m_attachments.begin();
    while (it != m_attachments.end())
    {
        AttachmentResource& attachment = *it->second;

        if (!attachment.IsUsed() && !attachment.IsPersistent())
        {
            attachment.Destroy();
            it = m_attachments.erase(it);
            continue;
        }

        if (!attachment.IsValid() || attachment.GetImage().NeedsRecreation())
        {
            attachment.Create();
        }

        ++it;
    }
}

void RenderGraph::DestroyAttachments()
{
    for (auto& [name, attachment] : m_attachments)
    {
        attachment->Destroy();
    }

    m_attachments.clear();
}

void RenderGraph::UpdateTextures()
{
    auto it = m_texturesFromAttachments.begin();
    while (it != m_texturesFromAttachments.end())
    {
        const std::string& name = it->first;
        TextureResource& texture = *(it->second);

        if (!texture.IsUsed() && !texture.IsPersistent())
        {
            texture.Destroy();
            it = m_texturesFromAttachments.erase(it);
            continue;
        }

        if (!texture.IsValid())
        {
            AttachmentResource& attachment = GetAttachmentResource(name);
            Assert(attachment.IsValid(), "Trying to create a texture from an invalid attachment %s", name.c_str());
            texture.CreateFromImage(attachment.GetImagePtr());
        }

        ++it;
    }
}

void RenderGraph::DestroyTextures()
{
    for (auto& [name, texture] : m_texturesFromAttachments)
    {
        texture->Destroy();
    }

    m_texturesFromAttachments.clear();
}

void RenderGraph::DestroyRenderPassesPendingRemoval()
{
    uint16_t const currentFrame = Renderer::GetInstance().GetCurrentFrame();
    
    size_t i = 0;
    while (i < m_renderPassesToRemove.size())
    {
        RenderPassPendingRemoval& removalInfo = m_renderPassesToRemove[i];

        // Remove the pass from the main vector, but move it first to prevent it from being destroyed
        auto const& it = std::find_if(m_renderPasses.begin(), m_renderPasses.end(), 
            [&removalInfo](UniquePtr<RenderPass>& pass) -> bool
            {
                return pass->GetName() == removalInfo.m_name;
            }
        );

        if (it != m_renderPasses.end())
        {
            removalInfo.m_pass = std::move(*it);
            m_renderPasses.erase(it);
        }
        
        if (currentFrame == removalInfo.m_frameId)
        {
            // We can safely terminate the pass now
            removalInfo.m_pass->Terminate();
            m_renderPassesToRemove.erase(m_renderPassesToRemove.begin() + i);
        }
        else
        {
            ++i;
        }
    }
}

void RenderGraph::TerminateRenderPasses()
{
    for (UniquePtr<RenderPass>& pass : m_renderPasses)
    {
        pass->Terminate();
    }

    m_renderPasses.clear();
    m_renderPassesToRemove.clear();
}

void RenderGraph::CreateCommandPool()
{
    Renderer& renderer = Renderer::GetInstance();

    VkCommandPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = renderer.GetPhysicalDeviceInfo().m_graphicsQueueFamily.value();

    if (vkCreateCommandPool(renderer.GetDevice(), &poolInfo, nullptr, &m_commandPool) != VK_SUCCESS)
    {
        ThrowError("Failed to create command pool.");
    }
}

void RenderGraph::CreateCommandBuffers()
{
    Renderer& renderer = Renderer::GetInstance();

    m_commandBuffers.resize(renderer.GetNumberOfSwapchainImages());

    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandPool = m_commandPool;
    allocInfo.commandBufferCount = m_commandBuffers.size();

    if (vkAllocateCommandBuffers(renderer.GetDevice(), &allocInfo, m_commandBuffers.data()) != VK_SUCCESS)
    {
        ThrowError("Failed to allocate command buffers.");
    }
}

void RenderGraph::DestroyCommandPool()
{
    Renderer& renderer = Renderer::GetInstance();

    vkDestroyCommandPool(renderer.GetDevice(), m_commandPool, nullptr);

    m_commandBuffers.clear();
}

void RenderGraph::AddPass(UniquePtr<RenderPass>& pass)
{
    auto const& it = std::find_if(m_renderPasses.begin(), m_renderPasses.end(), 
        [&pass](UniquePtr<RenderPass>& otherPass) -> bool
        {
            return pass->GetName() == otherPass->GetName();
        }
    );

    if (it == m_renderPasses.end())
    {
        m_renderPasses.push_back(std::move(pass));
        m_renderPasses.back()->Init();
    }
}

void RenderGraph::RemovePass(std::string const& name)
{
    auto const& it = std::find_if(m_renderPassesToRemove.begin(), m_renderPassesToRemove.end(), 
        [&name](RenderPassPendingRemoval const& removalInfo) -> bool
        {
            return removalInfo.m_name == name;
        }
    );

    auto const& renderPassit = std::find_if(m_renderPasses.begin(), m_renderPasses.end(), 
        [&name](UniquePtr<RenderPass>& otherPass) -> bool
        {
            return name == otherPass->GetName();
        }
    );

    if (it == m_renderPassesToRemove.end() && renderPassit != m_renderPasses.end())
    {
        RenderPassPendingRemoval& removalInfo = m_renderPassesToRemove.emplace_back();
        removalInfo.m_name = name;
        removalInfo.m_frameId = Renderer::GetInstance().GetCurrentFrame();
    }
}

void RenderGraph::OnSwapchainRecreated(VkExtent2D const&)
{
    for (auto& [_, attachment] : m_attachments)
    {
        ImageResource& image = attachment->GetImage();
        if (image.IsValid() && image.GetCreationInfo().m_sizeType == SizeType::RelativeToSwapchain)
        {
            image.SetNeedsRecreation();
        }    
    }
}

void PresentationRenderGraph::Init()
{
    AttachmentResource& backbuffer = GetAttachmentResource("backbuffer");
    backbuffer.GetImage().AddImageUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT);

    if (Renderer::GetInstance().GetRenderSettings().m_useMultisampling)
    {
        AttachmentResource& intermediateResolve = GetAttachmentResource("resolve");
        AttachmentCreationInfo intermediateResolveInfo;
        intermediateResolveInfo.m_imageCreateInfo.m_format = Renderer::GetInstance().ChooseBackbufferFormat();
        intermediateResolve.SetCreationInfo(intermediateResolveInfo);
        intermediateResolve.SetIsPersistent(true);
        intermediateResolve.GetImage().AddImageUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT);
    }

    RenderGraph::Init();
}

void PresentationRenderGraph::ExecuteInternal(VkCommandBuffer commandBuffer)
{
    RenderGraph::ExecuteInternal(commandBuffer);

    BlitBackbufferToSwapchainImage(commandBuffer);
}

void PresentationRenderGraph::BlitBackbufferToSwapchainImage(VkCommandBuffer commandBuffer)
{
    Renderer& renderer = Renderer::GetInstance();
    AttachmentResource& backbuffer = GetAttachmentResource("backbuffer");
    ImageResource& backbufferImage = backbuffer.GetImage();
    VkExtent2D const backbufferExtent = backbufferImage.GetExtent();
    VkExtent2D const swapchainExtent = renderer.GetSwapchainExtent();
    VkImageAspectFlags const backbufferAspectFlags = Renderer::GetAspectFlagsFromFormat(backbuffer.GetImageCreationInfo().m_format);
    AttachmentResource* bufferToBlitFrom = &backbuffer;

    backbufferImage.TransitionLayout(
        VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, 
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        commandBuffer
    );

    ImageResource& swapchainImage = renderer.GetCurrentSwapchainImage();
    swapchainImage.TransitionLayout(
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
        commandBuffer);

    // Resolve multisampling
    if (renderer.GetRenderSettings().m_useMultisampling)
    {
        AttachmentResource& intermediateResolve = GetAttachmentResource("resolve");
        ImageResource& intermediateResolveImage = intermediateResolve.GetImage();
        bufferToBlitFrom = &intermediateResolve;

        intermediateResolveImage.TransitionLayout(
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            commandBuffer);

        VkImageResolve resolveRegion = {};
        resolveRegion.srcOffset = {0, 0, 0};
        resolveRegion.srcSubresource.aspectMask = backbufferAspectFlags;
        resolveRegion.srcSubresource.mipLevel = 0;
        resolveRegion.srcSubresource.baseArrayLayer = 0;
        resolveRegion.srcSubresource.layerCount = 1;
        resolveRegion.dstOffset = {0, 0, 0};
        resolveRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        resolveRegion.dstSubresource.mipLevel = 0;
        resolveRegion.dstSubresource.baseArrayLayer = 0;
        resolveRegion.dstSubresource.layerCount = 1;
        resolveRegion.extent = {backbufferExtent.width, backbufferExtent.height, 1};

        vkCmdResolveImage(commandBuffer,
            backbufferImage.GetImage(), backbufferImage.GetCurrentLayout(),
            intermediateResolveImage.GetImage(), intermediateResolveImage.GetCurrentLayout(),
            1, &resolveRegion);

        intermediateResolveImage.TransitionLayout(
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
            VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_TRANSFER_BIT,
            commandBuffer);
    }

    // Blit
    {
        VkImageBlit blitRegion = {};
        blitRegion.srcOffsets[0] = {0, 0, 0};
        blitRegion.srcOffsets[1] = {(int32_t)backbufferExtent.width, (int32_t)backbufferExtent.height, 1};
        blitRegion.srcSubresource.aspectMask = backbufferAspectFlags;
        blitRegion.srcSubresource.mipLevel = 0;
        blitRegion.srcSubresource.baseArrayLayer = 0;
        blitRegion.srcSubresource.layerCount = 1;
        blitRegion.dstOffsets[0] = {0, 0, 0};
        blitRegion.dstOffsets[1] = {(int32_t)swapchainExtent.width, (int32_t)swapchainExtent.height, 1};
        blitRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        blitRegion.dstSubresource.mipLevel = 0;
        blitRegion.dstSubresource.baseArrayLayer = 0;
        blitRegion.dstSubresource.layerCount = 1;

        vkCmdBlitImage(commandBuffer,
            bufferToBlitFrom->GetImage().GetImage(), bufferToBlitFrom->GetImage().GetCurrentLayout(),
            swapchainImage.GetImage(), swapchainImage.GetCurrentLayout(),
            1, &blitRegion,
            VK_FILTER_LINEAR);
    }

    swapchainImage.TransitionLayout(
        VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
        commandBuffer);
}
