#pragma once

#include <Utilities/Helpers.hpp>

class AttachmentResource;
class RenderGraph;
class TextureResource;
struct AttachmentCreationInfo;
struct ExecutionContext;
struct TextureCreationInfo;

struct PassExecutionContext
{
    VkRenderingInfo m_renderingInfo;
    std::vector<VkRenderingAttachmentInfo> m_colorAttachments;
    std::optional<VkRenderingAttachmentInfo> m_depthAttachment;
    std::optional<VkRenderingAttachmentInfo> m_stencilAttachment;
};

class RenderPass
{
public:
    RenderPass(std::string const& name, RenderGraph* renderGraph);
    RenderPass(RenderPass const& other) = delete;
    RenderPass& operator=(RenderPass const& other) = delete;
    virtual ~RenderPass() = default;

    virtual void Init();
    void Execute(VkCommandBuffer commandBuffer, ExecutionContext& context);
    virtual void Terminate();

    inline std::string const& GetName() const { return m_name; }
    std::set<SharedPtr<AttachmentResource>> const& GetAttachments() const { return m_uniqueAttachments; }
    inline void SetRenderGraph(RenderGraph* renderGraph) { m_renderGraph = renderGraph; }

    void AddColorOutputAttachment(std::string const& name, AttachmentCreationInfo const& attachmentInfo);
    void SetDepthStencilInputAttachment(std::string const& name);
    void SetDepthStencilOutputAttachment(std::string const& name, AttachmentCreationInfo const& attachmentInfo);
    void AddTextureRead(std::string const& name, TextureCreationInfo const& textureInfo, bool persistent = false);
    void AddImageCopySource(std::string const& name);
    virtual void DeclareAttachmentsUsage() = 0;

    std::vector<AttachmentResource*> const& GetColorOutputAttachments() const { return m_colorOutputAttachments; }
    AttachmentResource const* GetDepthStencilAttachment() const { return m_depthStencilAttachment; }

protected:
    virtual void PreExecute(VkCommandBuffer commandBuffer, ExecutionContext const& context, PassExecutionContext& passContext);
    virtual void ExecuteInternal(VkCommandBuffer commandBuffer, PassExecutionContext const& context) = 0;
    virtual void PostExecute(VkCommandBuffer commandBuffer, ExecutionContext& context);

protected:
    RenderGraph* m_renderGraph = nullptr;
    
private:
    uint64_t const m_id = UINT64_MAX;
    std::string m_name;

    std::vector<AttachmentResource*> m_colorOutputAttachments;
    AttachmentResource* m_depthStencilAttachment = nullptr;
    
    std::set<SharedPtr<AttachmentResource>> m_uniqueAttachments;
    std::set<SharedPtr<TextureResource>> m_texturesFromAttachments;

    static uint64_t ms_nextId;
};
