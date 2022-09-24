#pragma once

#include <Systems/RenderPasses/RenderPass.hpp>
#include <Utilities/Helpers.hpp>

class RenderGraph;
class TextureResource;

class PrefilterPass : public RenderPass
{
public:
    PrefilterPass(std::string const& name, RenderGraph* renderGraph);

    virtual void DeclareAttachmentsUsage() override;
    virtual void Init() override;
    virtual void Terminate() override;

protected:
    virtual void ExecuteInternal(VkCommandBuffer commandBuffer, PassExecutionContext const& context) override;

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;

    uint16_t m_resolution = 512;

    SharedPtr<TextureResource> m_prefilteredCube = nullptr;
};

struct PrefilterPushConstantBlock
{
    glm::mat4 m_mvp;
    float m_roughness = 1.0f;
    uint32_t m_samples = 32;
};
