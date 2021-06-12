#include "Skybox.hpp"
#include "Model.hpp"
#include "Renderer.hpp"
#include "Camera.hpp"
#include "Utils.hpp"


Skybox::Skybox(Texture3D* texture)
{
    m_model = Renderer::GetInstance().m_cube;
    m_textureCube = texture;

    Init();
}

Skybox::~Skybox()
{
    Cleanup();
}

void Skybox::Init()
{
    CreateUniformBuffers();
    CreateDescriptors();
    CreatePipeline();
}

void Skybox::Cleanup()
{
	Renderer& renderer = Renderer::GetInstance();

    vkDestroyPipeline(renderer.m_device, m_pipeline, nullptr);
    vkDestroyPipelineLayout(renderer.m_device, m_pipelineLayout, nullptr);

    for (size_t i = 0; i < m_uniformBuffers.size(); i++)
    {
        vkDestroyBuffer(renderer.m_device, m_uniformBuffers[i], nullptr);
        vkFreeMemory(renderer.m_device, m_uniformBuffersMemory[i], nullptr);
    }

    vkDestroyDescriptorSetLayout(renderer.m_device, m_descriptorSetLayout, nullptr);
    vkDestroyDescriptorPool(renderer.m_device, m_descriptorPool, nullptr);
}

void Skybox::OnSwapchainRecreation()
{
    Cleanup();
    Init();
}

void Skybox::CreateUniformBuffers()
{
	Renderer& renderer = Renderer::GetInstance();

    m_uniformBuffers.resize(renderer.m_swapChainImages.size());
    m_uniformBuffersMemory.resize(renderer.m_swapChainImages.size());

    for (size_t i = 0; i < m_uniformBuffers.size(); i++)
    {
        renderer.CreateBuffer(sizeof(SkyboxMatrices), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_uniformBuffers[i], m_uniformBuffersMemory[i]);
        UpdateUniformBuffer(i);
    }
}

void Skybox::UpdateUniformBuffer(int16_t i)
{
	Renderer& renderer = Renderer::GetInstance();

    m_uboMatrices.m_model = glm::mat4(glm::mat3(renderer.m_camera->GetView()));
    m_uboMatrices.m_projection = renderer.m_camera->GetProjection();
    
    void* data;
    vkMapMemory(renderer.m_device, m_uniformBuffersMemory[i], 0, sizeof(m_uboMatrices), 0, &data);
        memcpy(data, &m_uboMatrices, sizeof(m_uboMatrices));
    vkUnmapMemory(renderer.m_device, m_uniformBuffersMemory[i]);
}

void Skybox::CreateDescriptors()
{
	Renderer& renderer = Renderer::GetInstance();

    m_descriptorSets.resize(renderer.m_swapChainImages.size());

    std::vector<VkDescriptorPoolSize> poolSizes = {
        { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, static_cast<uint32_t>(renderer.m_swapChainImages.size()) },
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, static_cast<uint32_t>(renderer.m_swapChainImages.size()) }
    };

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(renderer.m_swapChainImages.size() * 2);

    if (vkCreateDescriptorPool(renderer.m_device, &poolInfo, nullptr, &m_descriptorPool) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create skybox descriptor pool!");
    }

    // Descriptor layout
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {
        { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr }, // ubo matrices
        { 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }, // cubemap
    };

    VkDescriptorSetLayoutCreateInfo layoutInfo{};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pBindings = setLayoutBindings.data();
    layoutInfo.bindingCount = static_cast<uint32_t>(setLayoutBindings.size());
    if (vkCreateDescriptorSetLayout(renderer.m_device, &layoutInfo, nullptr, &m_descriptorSetLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create skybox descriptor set layout!");
    }


    // Descriptor sets
    for (size_t i = 0; i < renderer.m_swapChainImages.size(); i++)
    {
        VkDescriptorSetAllocateInfo descriptorSetAllocInfo{};
        descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        descriptorSetAllocInfo.descriptorPool = m_descriptorPool;
        descriptorSetAllocInfo.pSetLayouts = &m_descriptorSetLayout;
        descriptorSetAllocInfo.descriptorSetCount = 1;
        if (vkAllocateDescriptorSets(renderer.m_device, &descriptorSetAllocInfo, &m_descriptorSets[i]) != VK_SUCCESS)
        {
            throw std::runtime_error("failed to allocate skybox descriptor sets!");
        }

        // TODO - Create buffer object in a struct to hold its descriptor
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = m_uniformBuffers[i];
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(SkyboxMatrices);

        std::array<VkWriteDescriptorSet, 2> writeDescriptorSets{};

        writeDescriptorSets[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSets[0].descriptorCount = 1;
        writeDescriptorSets[0].dstSet = m_descriptorSets[i];
        writeDescriptorSets[0].dstBinding = 0;
        writeDescriptorSets[0].pBufferInfo = &bufferInfo;

        writeDescriptorSets[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSets[1].descriptorCount = 1;
        writeDescriptorSets[1].dstSet = m_descriptorSets[i];
        writeDescriptorSets[1].dstBinding = 1;
        writeDescriptorSets[1].pImageInfo = &m_textureCube->m_descriptor;

        vkUpdateDescriptorSets(renderer.m_device, static_cast<uint32_t>(writeDescriptorSets.size()), writeDescriptorSets.data(), 0, nullptr);
    }
}

void Skybox::CreatePipeline()
{
	Renderer& renderer = Renderer::GetInstance();

    std::string vertShaderCode = Utils::ReadFile("shaders/_spirv_/skybox.vert.spv");
    std::string fragShaderCode = Utils::ReadFile("shaders/_spirv_/skybox.frag.spv");

    VkShaderModule vertShaderModule = renderer.CreateShaderModule(vertShaderCode);
    VkShaderModule fragShaderModule = renderer.CreateShaderModule(fragShaderCode);

    VkPipelineShaderStageCreateInfo vertShaderStageInfo = {};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule;
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo = {};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule;
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = {vertShaderStageInfo, fragShaderStageInfo};

    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

    VkVertexInputBindingDescription bindingDescription = Skybox::Vertex::GetBindingDescription();
    std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions = Skybox::Vertex::GetAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();

    VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport = {};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = (float) renderer.m_swapChainExtent.width;
    viewport.height = (float) renderer.m_swapChainExtent.height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;

    VkRect2D scissor = {};
    scissor.offset = {0, 0};
    scissor.extent = renderer.m_swapChainExtent;

    VkPipelineViewportStateCreateInfo viewportState = {};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.pViewports = &viewport;
    viewportState.scissorCount = 1;
    viewportState.pScissors = &scissor;

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
    multisampling.sampleShadingEnable = VK_FALSE;
    if (g_settings.m_multiSampling)
    {
        multisampling.rasterizationSamples = g_settings.m_sampleCount;
    }

    VkPipelineDepthStencilStateCreateInfo depthStencil = {};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_FALSE;
    depthStencil.depthWriteEnable = VK_FALSE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending = {};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;

    // Pipeline Layout
    std::vector<VkDescriptorSetLayout> const setLayouts = { m_descriptorSetLayout };
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(setLayouts.size());
    pipelineLayoutInfo.pSetLayouts = setLayouts.data();

    if (vkCreatePipelineLayout(renderer.m_device, &pipelineLayoutInfo, nullptr, &m_pipelineLayout) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create skybox pipeline layout!");
    }

    // Pipeline
    VkGraphicsPipelineCreateInfo pipelineInfo = {};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.layout = m_pipelineLayout;
    pipelineInfo.renderPass = renderer.m_renderPass;
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    if (vkCreateGraphicsPipelines(renderer.m_device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &m_pipeline) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create skybox pipeline!");
    }

    vkDestroyShaderModule(renderer.m_device, fragShaderModule, nullptr);
    vkDestroyShaderModule(renderer.m_device, vertShaderModule, nullptr);
}

void Skybox::Draw(int16_t i)
{
	Renderer& renderer = Renderer::GetInstance();

    vkCmdBindDescriptorSets(renderer.m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipelineLayout, 0, 1, &m_descriptorSets[i], 0, nullptr);
    vkCmdBindPipeline(renderer.m_commandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_pipeline);
    m_model->Draw(renderer.m_commandBuffers[i]);
}

VkVertexInputBindingDescription Skybox::Vertex::GetBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Model::Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 1> Skybox::Vertex::GetAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 1> attributeDescriptions = {};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Model::Vertex, m_position);

    return attributeDescriptions;
}