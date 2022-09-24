#pragma once

#include <Systems/RenderPasses/RenderPass.hpp>
#include <Resources/AttachmentResource.hpp>
#include <Resources/TextureResource.hpp>
#include <Utilities/Helpers.hpp>
#include <Utilities/Observer.hpp>

struct ExecutionContext
{
    std::unordered_set<const AttachmentResource*> m_uniqueColorAttachments;
    std::unordered_set<const AttachmentResource*> m_uniqueDepthStencilAttachments;  
};

struct RenderPassPendingRemoval
{
    RenderPassPendingRemoval() = default;
    RenderPassPendingRemoval(RenderPassPendingRemoval const&) = delete;
    RenderPassPendingRemoval& operator=(RenderPassPendingRemoval const&) = delete;
    RenderPassPendingRemoval(RenderPassPendingRemoval&&) = default;
    RenderPassPendingRemoval& operator=(RenderPassPendingRemoval&&) noexcept = default;
    ~RenderPassPendingRemoval() = default;

    std::string m_name;
    uint16_t m_frameId = 0;
    UniquePtr<RenderPass> m_pass;
};

class RenderGraph : public SwapchainObserver
{
public:
    virtual void Init();
    void Execute(VkCommandBuffer commandBuffer);
    virtual void Terminate();

    template<typename PASS, typename... ARGS>
    void AddPass(std::string const& name, ARGS&&... args);
    void AddPass(UniquePtr<RenderPass>& pass);
    void RemovePass(std::string const& name);

    virtual void OnSwapchainRecreated(VkExtent2D const& newExtent) override;

    // Getters
    inline VkCommandBuffer& GetCommandBuffer(uint32_t const frameIndex) { return m_commandBuffers[frameIndex]; }
    inline AttachmentResource& GetAttachmentResource(std::string const& name) { return *m_attachments[name]; }
    inline TextureResource& GetTextureFromAttachmentResource(std::string const& name) { return *m_texturesFromAttachments[name]; }

protected:
    virtual void ExecuteInternal(VkCommandBuffer commandBuffer);

private:
    // Create
    void CreateCommandPool();
    void CreateCommandBuffers();

    // Destroy
    void DestroyCommandPool();

    void BeginCommandBuffer(VkCommandBuffer commandBuffer);
    void EndCommandBuffer(VkCommandBuffer commandBuffer);

    void UpdateAttachments();
    void DestroyAttachments();

    void UpdateTextures();
    void DestroyTextures();

    void DestroyRenderPassesPendingRemoval();
    void TerminateRenderPasses();

private:
    std::vector<UniquePtr<RenderPass>> m_renderPasses;
    std::unordered_map<std::string, DefaultedSharedPtr<AttachmentResource>> m_attachments;
    std::unordered_map<std::string, DefaultedSharedPtr<TextureResource>> m_texturesFromAttachments;

    VkCommandPool m_commandPool = VK_NULL_HANDLE;
    std::vector<VkCommandBuffer> m_commandBuffers;

    std::vector<RenderPassPendingRemoval> m_renderPassesToRemove;
};

template<typename PASS, typename... ARGS>
void RenderGraph::AddPass(std::string const& name, ARGS&&... args)
{
    UniquePtr<RenderPass> pass = std::make_unique<PASS>(name, this, std::forward<ARGS>(args)...);
    AddPass(pass);
}

class PresentationRenderGraph : public RenderGraph
{
public:
    virtual void Init() override;

protected:
    virtual void ExecuteInternal(VkCommandBuffer commandBuffer) override;

private:
    void BlitBackbufferToSwapchainImage(VkCommandBuffer commandBuffer);
};
