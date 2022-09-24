#pragma once

#include <Systems/RenderPasses/RenderPass.hpp>

class RenderGraph;

class BrdflutPass : public RenderPass
{
public:
    BrdflutPass(std::string const& name, RenderGraph* renderGraph) : RenderPass(name, renderGraph) {}

    virtual void DeclareAttachmentsUsage() override;
    virtual void Init() override;
    virtual void Terminate() override;

protected:
    virtual void ExecuteInternal(VkCommandBuffer commandBuffer, PassExecutionContext const& context) override;

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;

    uint16_t m_resolution = 512;
};
