#include <Systems/RenderPasses/PrefilterPass.hpp>

#include <Components/IBLComponent.hpp>
#include <Components/StaticMeshComponent.hpp>
#include <Components/SkyboxComponent.hpp>
#include <Resources/AttachmentResource.hpp>
#include <Resources/Descriptor.hpp>
#include <Resources/ImageResource.hpp>
#include <Resources/ResourceHandle.hpp>
#include <Resources/TextureResource.hpp>
#include <Systems/EntitySystem.hpp>
#include <Systems/Renderer.hpp>
#include <Systems/RenderGraph.hpp>
#include <Systems/ResourceManager.hpp>

PrefilterPass::PrefilterPass(std::string const& name, RenderGraph* renderGraph)
    : RenderPass(name, renderGraph)
{
    TextureCreationInfo prefilteredCubeInfo;
    prefilteredCubeInfo.m_imageCreateInfo.m_format = VK_FORMAT_R16G16B16A16_SFLOAT;
    prefilteredCubeInfo.m_imageCreateInfo.m_width = m_resolution;
    prefilteredCubeInfo.m_imageCreateInfo.m_height = m_resolution;
    prefilteredCubeInfo.m_imageCreateInfo.m_sizeType = SizeType::Absolute;
    prefilteredCubeInfo.m_imageCreateInfo.m_mipLevels = floor(log2(m_resolution)) + 1;
    prefilteredCubeInfo.m_imageCreateInfo.m_layers = 6;
    prefilteredCubeInfo.m_samplerInfo.m_addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    prefilteredCubeInfo.m_samplerInfo.m_addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    prefilteredCubeInfo.m_samplerInfo.m_addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    prefilteredCubeInfo.m_samplerInfo.m_anisotropy = 1.0f;
    m_prefilteredCube = std::make_shared<TextureResource>(prefilteredCubeInfo);

    EntitySystem& entitySystem = EntitySystem::GetInstance();
    IBLComponent& ibl = entitySystem.GetOrAddComponent<IBLComponent>(entitySystem.GetGlobalEntity());
    ibl.SetPrefiltered(m_prefilteredCube);
}

void PrefilterPass::DeclareAttachmentsUsage()
{
    AttachmentCreationInfo prefilteredAttachmentInfo;
    prefilteredAttachmentInfo.m_imageCreateInfo.m_format = VK_FORMAT_R16G16B16A16_SFLOAT;
    prefilteredAttachmentInfo.m_imageCreateInfo.m_width = m_resolution;
    prefilteredAttachmentInfo.m_imageCreateInfo.m_height = m_resolution;
    prefilteredAttachmentInfo.m_imageCreateInfo.m_sizeType = SizeType::Absolute;
    AddColorOutputAttachment("tempPrefiltered", prefilteredAttachmentInfo);
    AddImageCopySource("tempPrefiltered");
}

void PrefilterPass::Init()
{
    RenderPass::Init();

    Renderer& renderer = Renderer::GetInstance();
    ResourceManager& resourceManager = ResourceManager::GetInstance();

    // Create pipeline
    VkShaderModule vertexShaderModule = resourceManager.GetShaderModule("FilterCube.vert");
    VkShaderModule fragmentShaderModule = resourceManager.GetShaderModule("PrefilterCube.frag");

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

    VkVertexInputAttributeDescription attributeDescription = {};
    attributeDescription.binding = 0;
    attributeDescription.location = 0;
    attributeDescription.format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescription.offset = 0;

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = 1;
    vertexInputInfo.pVertexAttributeDescriptions = &attributeDescription;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    std::vector<VkDynamicState> const dynamicStateEnables = { VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR };
    VkPipelineDynamicStateCreateInfo dynamicState = {};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.pDynamicStates = dynamicStateEnables.data();
    dynamicState.dynamicStateCount = dynamicStateEnables.size();

    VkPipelineRasterizationStateCreateInfo rasterizer = {};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.cullMode = VK_CULL_MODE_NONE;
    rasterizer.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisampling = {};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

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
        resourceManager.GetDescriptorLayout(SkyboxComponentResource::ms_bindings)
    };

    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = setLayouts.size();
    pipelineLayoutInfo.pSetLayouts = setLayouts.data();

    VkPushConstantRange pushConstantRange = {};
    pushConstantRange.size = sizeof(PrefilterPushConstantBlock);
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

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
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
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

void PrefilterPass::ExecuteInternal(VkCommandBuffer commandBuffer, PassExecutionContext const& context)
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    auto const& skyboxView = entitySystem.GetView<SkyboxComponentResource const, StaticMeshComponent const>();
    Entity skyboxEntity = skyboxView.front();
    if (!EntitySystem::IsEntityValid(skyboxEntity))
    {
        return;
    }

    StaticMeshComponent const& staticMesh = skyboxView.Get<StaticMeshComponent const>(skyboxEntity);
    SkyboxComponentResource const& skyboxResource = skyboxView.Get<SkyboxComponentResource const>(skyboxEntity);

    VkBuffer const vertexBuffers[] = { staticMesh.GetVertexBuffer() };
    VkDeviceSize const offsets[] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);

    VkBuffer const indexBuffer = staticMesh.GetIndexBuffer();
    if (indexBuffer != VK_NULL_HANDLE)
    {
        vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
    }

    PrefilterPushConstantBlock pushBlock = {};

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

    std::vector<glm::mat4> const matrices = {
        glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::rotate(glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f)), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::rotate(glm::mat4(1.0f), glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::rotate(glm::mat4(1.0f), glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f)),
        glm::rotate(glm::mat4(1.0f), glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f)),
    };

    std::vector<VkDescriptorSet> const descriptorsets = {
        skyboxResource.m_descriptorSet.GetResource().GetDescriptorSet(),
    };

    m_prefilteredCube->GetImage().TransitionLayout(
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        commandBuffer
    );
    
    uint8_t const mipLevels = m_prefilteredCube->GetImageCreationInfo().m_mipLevels;
    for (uint8_t mipLevel = 0; mipLevel < mipLevels; ++mipLevel)
    {
        for (uint8_t face = 0; face < 6; ++face)
        {
            // Render scene from cube face's point of view
            vkCmdBeginRendering(commandBuffer, &context.m_renderingInfo);

            vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

            viewport.width = static_cast<float>(m_resolution * std::pow(0.5f, mipLevel));
            viewport.height = static_cast<float>(m_resolution * std::pow(0.5f, mipLevel));
            vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
            vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

            vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, descriptorsets.size(), descriptorsets.data(), 0, nullptr);

            pushBlock.m_mvp = glm::perspective((float)(M_PI / 2.0f), 1.0f, 0.1f, 512.0f) * matrices[face];
            pushBlock.m_roughness = (float)mipLevel / (float)(mipLevels - 1);
            vkCmdPushConstants(commandBuffer, m_pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(PrefilterPushConstantBlock), &pushBlock);

            for (Primitive const& primitive : staticMesh.GetPrimitives())
            {
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

            AttachmentResource& attachment = m_renderGraph->GetAttachmentResource("tempPrefiltered");
            ImageResource& attachmentImage = attachment.GetImage();

            VkImageLayout const initialLayout = attachmentImage.GetCurrentLayout();
            if (initialLayout != VK_IMAGE_LAYOUT_GENERAL)
            {
                attachmentImage.TransitionLayout(
                    VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                    commandBuffer
                );
            }

            // Copy region for transfer from framebuffer to cube face
            VkImageCopy copyRegion = {};

            copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.srcSubresource.baseArrayLayer = 0;
            copyRegion.srcSubresource.mipLevel = 0;
            copyRegion.srcSubresource.layerCount = 1;
            copyRegion.srcOffset = { 0, 0, 0 };

            copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            copyRegion.dstSubresource.baseArrayLayer = face;
            copyRegion.dstSubresource.mipLevel = mipLevel;
            copyRegion.dstSubresource.layerCount = 1;
            copyRegion.dstOffset = { 0, 0, 0 };

            copyRegion.extent.width = viewport.width;
            copyRegion.extent.height = viewport.height;
            copyRegion.extent.depth = 1;

            vkCmdCopyImage(
                commandBuffer,
                attachmentImage.GetImage(),
                attachmentImage.GetCurrentLayout(),
                m_prefilteredCube->GetImage().GetImage(),
                m_prefilteredCube->GetImage().GetCurrentLayout(),
                1,
                &copyRegion
            );

            if (initialLayout != VK_IMAGE_LAYOUT_GENERAL)
            {
                attachmentImage.TransitionLayout(
                    initialLayout,
                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                    VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
                    commandBuffer
                );
            }
        }
    }

    m_prefilteredCube->GetImage().TransitionLayout(
        VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
        commandBuffer
    );

    m_renderGraph->RemovePass(GetName());
}

void PrefilterPass::Terminate()
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
