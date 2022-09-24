#include <Systems/RenderPasses/RenderPass.hpp>

#include <Resources/AttachmentResource.hpp>
#include <Resources/ImageResource.hpp>
#include <Resources/TextureResource.hpp>
#include <Systems/Renderer.hpp>
#include <Systems/RenderGraph.hpp>

uint64_t RenderPass::ms_nextId = 0;

RenderPass::RenderPass(std::string const& name, RenderGraph* renderGraph)
    : m_renderGraph(renderGraph)
    , m_id(++ms_nextId)
    , m_name(name)
{
}

void RenderPass::AddColorOutputAttachment(std::string const& name, AttachmentCreationInfo const& attachmentInfo)
{
    AttachmentResource& attachment = m_renderGraph->GetAttachmentResource(name);
    ImageResource& image = attachment.GetImage();
    attachment.SetWrittenInPass(m_id);
    attachment.SetCreationInfo(attachmentInfo);
    image.AddImageUsageFlags(VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT);
    m_colorOutputAttachments.push_back(&attachment);
    m_uniqueAttachments.insert(attachment.GetSharedPtr());
}

void RenderPass::SetDepthStencilInputAttachment(std::string const& name)
{
    AttachmentResource& attachment = m_renderGraph->GetAttachmentResource(name);
    ImageResource& image = attachment.GetImage();
    attachment.SetReadInPass(m_id);
    image.AddImageUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    m_depthStencilAttachment = &attachment;
    m_uniqueAttachments.insert(attachment.GetSharedPtr());
}

void RenderPass::SetDepthStencilOutputAttachment(std::string const& name, AttachmentCreationInfo const& attachmentInfo)
{
    AttachmentResource& attachment = m_renderGraph->GetAttachmentResource(name);
    ImageResource& image = attachment.GetImage();
    attachment.SetWrittenInPass(m_id);
    attachment.SetCreationInfo(attachmentInfo);
    image.AddImageUsageFlags(VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
    m_depthStencilAttachment = &attachment;
    m_uniqueAttachments.insert(attachment.GetSharedPtr());
}

void RenderPass::AddTextureRead(std::string const& name, TextureCreationInfo const& textureInfo, bool persistent)
{
    AttachmentResource& attachment = m_renderGraph->GetAttachmentResource(name);
    ImageResource& image = attachment.GetImage();
    image.AddImageUsageFlags(VK_IMAGE_USAGE_SAMPLED_BIT);

    TextureResource& texture = m_renderGraph->GetTextureFromAttachmentResource(name);
    texture.SetReadInPass(m_id);
    texture.SetIsPersistent(persistent);
    texture.SetCreationInfo(textureInfo);
    m_texturesFromAttachments.insert(texture.GetSharedPtr());
}

void RenderPass::AddImageCopySource(std::string const& name)
{
    AttachmentResource& attachment = m_renderGraph->GetAttachmentResource(name);
    ImageResource& image = attachment.GetImage();
    attachment.SetWrittenInPass(m_id);
    image.AddImageUsageFlags(VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
    m_uniqueAttachments.insert(attachment.GetSharedPtr());
}

void RenderPass::Init()
{
    DeclareAttachmentsUsage();
}

void RenderPass::Terminate()
{
    for (SharedPtr<AttachmentResource> const& attachment : m_uniqueAttachments)
    {
        attachment->RemovePassUsage(m_id);
    }

    m_uniqueAttachments.clear();
    m_colorOutputAttachments.clear();
    m_depthStencilAttachment = nullptr;
    m_texturesFromAttachments.clear();
}

void RenderPass::Execute(VkCommandBuffer commandBuffer, ExecutionContext& context)
{
    PassExecutionContext passContext;
    PreExecute(commandBuffer, context, passContext);
    ExecuteInternal(commandBuffer, passContext);
    PostExecute(commandBuffer, context);
}

void RenderPass::PreExecute(VkCommandBuffer commandBuffer, ExecutionContext const& context, PassExecutionContext& passContext)
{
    Renderer& renderer = Renderer::GetInstance();

    VkExtent2D minExtent = { UINT32_MAX, UINT32_MAX };

    // Color attachments
    passContext.m_colorAttachments.reserve(m_colorOutputAttachments.size());
    for (AttachmentResource* attachment : m_colorOutputAttachments)
    {
        ImageResource& image = attachment->GetImage();
        ImageCreateInfo const& imageInfo = attachment->GetImageCreationInfo();

        VkExtent2D const attachmentExtent = image.GetExtent();
        minExtent.width = std::min(minExtent.width, attachmentExtent.width);
        minExtent.height = std::min(minExtent.height, attachmentExtent.height);

        VkRenderingAttachmentInfo attachmentInfo = {};
        attachmentInfo.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        attachmentInfo.imageView = image.GetImageView();
        attachmentInfo.clearValue = attachment->GetClearValue();

        if (!Contains(context.m_uniqueColorAttachments, attachment))
        {
            // First use of the color attachment, clear it
            attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
        else
        {
            // Not the first use, load it
            attachmentInfo.loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        }

        // Always store color outputs
        attachmentInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        VkFormat const swapchainFormat = renderer.GetSwapchainFormat();
        
        if (imageInfo.m_format == swapchainFormat)
        {
            // This is a swapchain image
            layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
        }
        else
        {
            // Use optimal layout
            layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        }

        attachmentInfo.imageLayout = layout;

        image.TransitionLayout(
            attachmentInfo.imageLayout, 
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
            commandBuffer
        );

        passContext.m_colorAttachments.push_back(attachmentInfo);
    }
    
    // Depth and Stencil attachments
    if (m_depthStencilAttachment)
    {
        ImageResource& image = m_depthStencilAttachment->GetImage();

        VkExtent2D const attachmentExtent = image.GetExtent();
        minExtent.width = std::min(minExtent.width, attachmentExtent.width);
        minExtent.height = std::min(minExtent.height, attachmentExtent.height);

        passContext.m_depthAttachment = VkRenderingAttachmentInfo();
        passContext.m_depthAttachment->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
        passContext.m_depthAttachment->imageView = image.GetImageView();
        passContext.m_depthAttachment->clearValue = m_depthStencilAttachment->GetClearValue();

        const bool hasStencil = renderer.FormatHasStencil(image.GetCreationInfo().m_format);

        if (!Contains(context.m_uniqueDepthStencilAttachments, m_depthStencilAttachment))
        {
            // First use of the color attachment, clear it
            passContext.m_depthAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        }
        else
        {
            // Not the first use, load it
            passContext.m_depthAttachment->loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
        }

        // Always store depth outputs
        passContext.m_depthAttachment->storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        
        VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
        
        if (m_depthStencilAttachment->IsWrittenInPass(m_id))
        {
            // Use optimal write layout
            layout = hasStencil ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL;
        }
        else
        {
            // Use optimal read layout
            layout = hasStencil ? VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL : VK_IMAGE_LAYOUT_DEPTH_READ_ONLY_OPTIMAL;
        }

        passContext.m_depthAttachment->imageLayout = layout;

        image.TransitionLayout(
            passContext.m_depthAttachment->imageLayout, 
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
            commandBuffer
        );

        if (hasStencil)
        {
            passContext.m_stencilAttachment = VkRenderingAttachmentInfo();
            passContext.m_stencilAttachment->sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            passContext.m_stencilAttachment->imageView = passContext.m_depthAttachment->imageView;
            passContext.m_stencilAttachment->clearValue = passContext.m_depthAttachment->clearValue;
            passContext.m_stencilAttachment->imageLayout = passContext.m_depthAttachment->imageLayout;
            passContext.m_stencilAttachment->loadOp = passContext.m_depthAttachment->loadOp;
            passContext.m_stencilAttachment->storeOp = passContext.m_depthAttachment->storeOp;
        }
    }

    // Texture reads
    for (SharedPtr<TextureResource> const& texture : m_texturesFromAttachments)
    {
        texture->GetImage().TransitionLayout(
            VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, 
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT,
            commandBuffer
        );
    }

    passContext.m_renderingInfo = {};
    passContext.m_renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
    passContext.m_renderingInfo.renderArea = { 0, 0, minExtent.width, minExtent.height };
    passContext.m_renderingInfo.layerCount = 1;
    passContext.m_renderingInfo.colorAttachmentCount = passContext.m_colorAttachments.size();
    passContext.m_renderingInfo.pColorAttachments = passContext.m_colorAttachments.data();
    passContext.m_renderingInfo.pDepthAttachment = passContext.m_depthAttachment ? &passContext.m_depthAttachment.value() : nullptr;
    passContext.m_renderingInfo.pStencilAttachment = passContext.m_stencilAttachment ? &passContext.m_stencilAttachment.value() : nullptr;
}

void RenderPass::PostExecute(VkCommandBuffer, ExecutionContext& context)
{
    context.m_uniqueColorAttachments.insert(m_colorOutputAttachments.begin(), m_colorOutputAttachments.end());

    if (m_depthStencilAttachment)
    {
        context.m_uniqueDepthStencilAttachments.insert(m_depthStencilAttachment);
    }
}
