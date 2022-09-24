#include <Components/StaticMeshComponent.hpp>

#include <Resources/TextureResource.hpp>
#include <Systems/Renderer.hpp>
#include <Systems/ResourceManager.hpp>

void StaticMeshComponent::SetVertices(std::vector<Vertex> const& vertices)
{
    m_vertices = vertices;

    if (vertices.empty())
    {
        ThrowError("Static Mesh has no vertices.");
        return;
    }
    
    // Create vertex buffer
    Renderer& renderer = Renderer::GetInstance();

    VkDeviceSize const bufferSize = sizeof(vertices.front()) * vertices.size();

    BufferInfo bufferInfo;
    bufferInfo.m_size = bufferSize;
    bufferInfo.m_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.m_memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

    Buffer stagingBuffer = Buffer(bufferInfo);
    stagingBuffer.CopyDataToBuffer(vertices.data(), bufferSize);

    bufferInfo.m_usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    bufferInfo.m_memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

    m_vertexBuffer = Buffer(bufferInfo);

    VkCommandBuffer commandBuffer = renderer.BeginSingleUseCommandBuffer();
    m_vertexBuffer.CopyDataFromBuffer(commandBuffer, stagingBuffer, bufferSize);
    renderer.EndSingleUseCommandBuffer(commandBuffer);
}

void StaticMeshComponent::SetIndices(std::vector<uint32_t> const& indices)
{
    m_indices = indices;

    if (indices.empty())
    {
        return;
    }
    
    // Create index buffer
    Renderer& renderer = Renderer::GetInstance();

    VkDeviceSize const bufferSize = sizeof(indices.front()) * indices.size();

    BufferInfo bufferInfo;
    bufferInfo.m_size = bufferSize;
    bufferInfo.m_usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
    bufferInfo.m_memoryUsage = VMA_MEMORY_USAGE_CPU_ONLY;

    Buffer stagingBuffer = Buffer(bufferInfo);
    stagingBuffer.CopyDataToBuffer(indices.data(), bufferSize);

    bufferInfo.m_usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    bufferInfo.m_memoryUsage = VMA_MEMORY_USAGE_GPU_ONLY;

    m_indexBuffer = Buffer(bufferInfo);

    VkCommandBuffer commandBuffer = renderer.BeginSingleUseCommandBuffer();
    m_indexBuffer.CopyDataFromBuffer(commandBuffer, stagingBuffer, bufferSize);
    renderer.EndSingleUseCommandBuffer(commandBuffer);
}

const std::vector<VkDescriptorSetLayoutBinding> Material::ms_bindings = {
    { 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }, // albedo
    { 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }, // physical
    { 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }, // normal
    { 3, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }, // ambient occlusion
    { 4, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }, // emission
};

void Material::Init()
{
    m_descriptorSet = CreateResourceHandle<DescriptorSet>(Material::ms_bindings);

    VkDescriptorImageInfo const& emptyTextureDescriptor = ResourceManager::GetInstance().GetEmptyTexture().GetDescriptorInfo();

    std::vector<VkDescriptorImageInfo> const imageDescriptors = {
        m_baseColorTexture ? m_baseColorTexture->GetDescriptorInfo() : emptyTextureDescriptor,
        m_metallicRoughnessTexture ? m_metallicRoughnessTexture->GetDescriptorInfo() : emptyTextureDescriptor,
        m_normalTexture ? m_normalTexture->GetDescriptorInfo() : emptyTextureDescriptor,
        m_occlusionTexture ? m_occlusionTexture->GetDescriptorInfo() : emptyTextureDescriptor,
        m_emissiveTexture ? m_emissiveTexture->GetDescriptorInfo() : emptyTextureDescriptor
    };

    std::array<VkWriteDescriptorSet, 5> writeDescriptorSets{};
    for (size_t i = 0; i < writeDescriptorSets.size(); i++)
    {
        writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSets[i].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        writeDescriptorSets[i].descriptorCount = 1;
        writeDescriptorSets[i].dstSet = m_descriptorSet.GetResource().GetDescriptorSet();
        writeDescriptorSets[i].dstBinding = i;
        writeDescriptorSets[i].pImageInfo = &imageDescriptors[i];
    }

    Renderer& renderer = Renderer::GetInstance();
    vkUpdateDescriptorSets(renderer.GetDevice(), writeDescriptorSets.size(), writeDescriptorSets.data(), 0, nullptr);
}

/*static*/ VkVertexInputBindingDescription Vertex::GetBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

/*static*/ std::array<VkVertexInputAttributeDescription, 4> Vertex::GetAttributeDescriptions()
{
    std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, m_position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, m_normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, m_uvSet0);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, m_uvSet1);

    return attributeDescriptions;
}
