#pragma once

#include <Systems/RenderPasses/RenderPass.hpp>

class RenderGraph;
class TextureResource;

class IrradiancePass : public RenderPass
{
public:
    IrradiancePass(std::string const& name, RenderGraph* renderGraph);

    virtual void DeclareAttachmentsUsage() override;
    virtual void Init() override;
    virtual void Terminate() override;

protected:
    virtual void ExecuteInternal(VkCommandBuffer commandBuffer, PassExecutionContext const& context) override;

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;

    uint16_t m_resolution = 64;
    float m_irradiancePhiSteps = 180.0f;
    float m_irradianceThetaSteps = 64.0f;

    SharedPtr<TextureResource> m_irradianceCube = nullptr;
};

struct IrradiancePushConstantBlock
{
    glm::mat4 m_mvp;
    float m_deltaPhi = 0.0f;
    float m_deltaTheta = 0.0f;
};
