#include <Systems/RenderPasses/SkyboxPass.hpp>

#include <Components/CameraComponent.hpp>
#include <Components/IBLComponent.hpp>
#include <Components/StaticMeshComponent.hpp>
#include <Components/SkyboxComponent.hpp>
#include <Resources/AttachmentResource.hpp>
#include <Resources/Buffer.hpp>
#include <Resources/Descriptor.hpp>
#include <Resources/ImageResource.hpp>
#include <Resources/ResourceHandle.hpp>
#include <Systems/EntitySystem.hpp>
#include <Systems/Renderer.hpp>
#include <Systems/ResourceManager.hpp>
#include <Utilities/Helpers.hpp>

void SkyboxPass::DeclareAttachmentsUsage()
{
    Renderer& renderer = Renderer::GetInstance();

    AttachmentCreationInfo colorAttachmentInfo;
    colorAttachmentInfo.m_imageCreateInfo.m_format = renderer.ChooseBackbufferFormat();
    colorAttachmentInfo.m_imageCreateInfo.m_sampleCount = renderer.GetRasterizationSampleCount();
    AddColorOutputAttachment("backbuffer", colorAttachmentInfo);

    AttachmentCreationInfo depthAttachmentInfo;
    depthAttachmentInfo.m_imageCreateInfo.m_format = renderer.ChooseDepthFormat(false);
    depthAttachmentInfo.m_imageCreateInfo.m_sampleCount = renderer.GetRasterizationSampleCount();
    SetDepthStencilOutputAttachment("depth", depthAttachmentInfo);
}

void SkyboxPass::Init()
{
    RenderPass::Init();

    Renderer& renderer = Renderer::GetInstance();
    ResourceManager& resourceManager = ResourceManager::GetInstance();

    // Create pipeline
    VkShaderModule vertexShaderModule = resourceManager.GetShaderModule("Skybox.vert");
    VkShaderModule fragmentShaderModule = resourceManager.GetShaderModule("Skybox.frag");    

    VkPipelineShaderStageCreateInfo vertexShaderStageInfo = {};
    vertexShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertexShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertexShaderStageInfo.module = vertexShaderModule;
    vertexShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragmentShaderStageInfo = {};
    fragmentShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragmentShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragmentShaderStageInfo.module = fragmentShaderModule;
    fragmentShaderStageInfo.pName = "main";

    std::array<VkPipelineShaderStageCreateInfo, 2> const shaderStages = { vertexShaderStageInfo, fragmentShaderStageInfo };

    VkVertexInputBindingDescription const& bindingDescription = Vertex::GetBindingDescription();
    std::array<VkVertexInputAttributeDescription, 4> const& attributeDescriptions = Vertex::GetAttributeDescriptions();

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = attributeDescriptions.size();
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = nullptr;
    viewportState.scissorCount = 1;
    viewportState.pScissors = nullptr;

    std::vector<VkDynamicState> dynamicEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = dynamicEnables.size();
    dynamicState.pDynamicStates = dynamicEnables.data();

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.rasterizationSamples = renderer.GetRasterizationSampleCount();
    multisampling.sampleShadingEnable = renderer.GetRenderSettings().m_useSampleShading;
    multisampling.minSampleShading = multisampling.sampleShadingEnable ? 1.0f : 0.0f;

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendState = {};
    colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendState.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendState;

    // Pipeline Layout
    std::vector<VkDescriptorSetLayout> const setLayouts = {
        resourceManager.GetDescriptorLayout(CameraComponentResource::ms_bindings),
        resourceManager.GetDescriptorLayout(IBLComponent::ms_bindings)
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = setLayouts.size();
    pipelineLayoutInfo.pSetLayouts = setLayouts.data();

    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    if (vkCreatePipelineLayout(renderer.GetDevice(), &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        ThrowError("Failed to create pipeline layout.");
    }

    // Pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = shaderStages.size();
    pipelineInfo.pStages = shaderStages.data();
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    std::vector<VkFormat> colorAttachmentFormats;
    colorAttachmentFormats.reserve(GetColorOutputAttachments().size());
    for (AttachmentResource const* attachment : GetColorOutputAttachments())
    {
        colorAttachmentFormats.push_back(attachment->GetImageCreationInfo().m_format);
    }

    VkFormat const depthFormat = GetDepthStencilAttachment() ? GetDepthStencilAttachment()->GetImageCreationInfo().m_format : VK_FORMAT_UNDEFINED;
    VkFormat const stencilFormat = GetDepthStencilAttachment() && renderer.FormatHasStencil(depthFormat) ? depthFormat : VK_FORMAT_UNDEFINED;

    VkPipelineRenderingCreateInfo pipelineRenderingInfo = {};
    pipelineRenderingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RENDERING_CREATE_INFO;
    pipelineRenderingInfo.colorAttachmentCount = colorAttachmentFormats.size();
    pipelineRenderingInfo.pColorAttachmentFormats = colorAttachmentFormats.data();
    pipelineRenderingInfo.depthAttachmentFormat = depthFormat;
    pipelineRenderingInfo.stencilAttachmentFormat = stencilFormat;
    PNextChainPushBack(&pipelineInfo, &pipelineRenderingInfo);

    if (vkCreateGraphicsPipelines(renderer.GetDevice(), renderer.GetPipelineCache(), 1, &pipelineInfo, nullptr, &m_graphicsPipeline) != VK_SUCCESS)
    {
        ThrowError("Failed to create graphics pipeline.");
    }
}

void SkyboxPass::ExecuteInternal(VkCommandBuffer commandBuffer, PassExecutionContext const& context)
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();

    auto const& cameraView = entitySystem.GetView<CameraComponentResource const>();
    Entity cameraEntity = cameraView.front();
    if (!EntitySystem::IsEntityValid(cameraEntity))
    {
        return;
    }

    auto const& skyboxView = entitySystem.GetView<SkyboxComponentResource const, StaticMeshComponent const>();
    Entity skyboxEntity = skyboxView.front();
    if (!EntitySystem::IsEntityValid(skyboxEntity))
    {
        return;
    }

    CameraComponentResource const& cameraResource = cameraView.Get<CameraComponentResource const>(cameraEntity);
    StaticMeshComponent const& staticMesh = skyboxView.Get<StaticMeshComponent const>(skyboxEntity);
    IBLComponent const& iblComponent = entitySystem.GetComponent<IBLComponent>(entitySystem.GetGlobalEntity());

    vkCmdBeginRendering(commandBuffer, &context.m_renderingInfo);

    VkViewport viewport = {};
    viewport.x = static_cast<float>(context.m_renderingInfo.renderArea.offset.x);
    viewport.y = static_cast<float>(context.m_renderingInfo.renderArea.offset.y);
    viewport.width = static_cast<float>(context.m_renderingInfo.renderArea.extent.width);
    viewport.height = static_cast<float>(context.m_renderingInfo.renderArea.extent.height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor = context.m_renderingInfo.renderArea;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

    VkBuffer const vertexBuffers[] = { staticMesh.GetVertexBuffer() };
    VkDeviceSize const offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    VkBuffer const indexBuffer = staticMesh.GetIndexBuffer();
    if (indexBuffer != VK_NULL_HANDLE)
    {
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    for (Primitive const& primitive : staticMesh.GetPrimitives())
    {
        std::vector<VkDescriptorSet> const descriptorsets = {
            cameraResource.m_descriptorSet.GetResource().GetDescriptorSet(),
            iblComponent.m_descriptorSet.GetResource().GetDescriptorSet(),
        };
        vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, descriptorsets.size(), descriptorsets.data(), 0, nullptr);

        if (primitive.m_hasIndices)
        {
            vkCmdDrawIndexed(commandBuffer, primitive.m_indexCount, 1, primitive.m_firstIndex, 0, 0);
        }
        else
        {
            vkCmdDraw(commandBuffer, primitive.m_vertexCount, 1, 0, 0);
        }
    }

    vkCmdEndRendering(commandBuffer);
}

void SkyboxPass::Terminate()
{
    Renderer& renderer = Renderer::GetInstance();

    if (m_pipelineLayout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(renderer.GetDevice(), m_pipelineLayout, nullptr);
        m_pipelineLayout = VK_NULL_HANDLE;
    }

    if (m_graphicsPipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(renderer.GetDevice(), m_graphicsPipeline, nullptr);
        m_graphicsPipeline = VK_NULL_HANDLE;
    }

    RenderPass::Terminate();
}
