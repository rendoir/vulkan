#pragma once

#include <Systems/RenderPasses/RenderPass.hpp>

class RenderGraph;

class ShadingPass : public RenderPass
{
public:
    ShadingPass(std::string const& name, RenderGraph* renderGraph) : RenderPass(name, renderGraph) {}

    virtual void DeclareAttachmentsUsage() override;
    virtual void Init() override;
    virtual void Terminate() override;

protected:
    virtual void ExecuteInternal(VkCommandBuffer commandBuffer, PassExecutionContext const& context) override;

private:
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;
    VkPipeline m_graphicsPipeline = VK_NULL_HANDLE;
};

struct MaterialPushConstantBlock
{
    glm::vec4 m_baseColorFactor;
    glm::vec3 m_emissiveFactor;
    int32_t m_colorTextureSet;
    int32_t m_physicalTextureSet;
    int32_t m_normalTextureSet;
    int32_t m_occlusionTextureSet;
    int32_t m_emissiveTextureSet;
    float m_metallicFactor;
    float m_roughnessFactor;
    float m_alphaMask;
    float m_alphaMaskCutoff;
};
