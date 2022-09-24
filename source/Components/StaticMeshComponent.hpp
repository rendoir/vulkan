#pragma once

#include <Components/EntityComponent.hpp>
#include <Resources/Buffer.hpp>
#include <Resources/Descriptor.hpp>
#include <Resources/ResourceHandle.hpp>
#include <Utilities/Helpers.hpp>

class TextureResource;

struct Material
{
    enum class AlphaMode : uint8_t
    { 
        Opaque,
        Mask,
        Blend
    };
    
    float m_metallicFactor  = 1.0f;
    float m_roughnessFactor = 1.0f;
    glm::vec4 m_baseColorFactor = glm::vec4(1.0f);
    glm::vec3 m_emissiveFactor  = glm::vec3(0.0f);
    
    SharedPtr<TextureResource> m_baseColorTexture = nullptr;
    SharedPtr<TextureResource> m_metallicRoughnessTexture = nullptr;
    SharedPtr<TextureResource> m_normalTexture = nullptr;
    SharedPtr<TextureResource> m_occlusionTexture = nullptr;
    SharedPtr<TextureResource> m_emissiveTexture = nullptr;

    uint8_t m_baseColorTextCoordSet = 0;
    uint8_t m_metallicRoughnessTextCoordSet= 0;
    uint8_t m_normalTextCoordSet = 0;
    uint8_t m_occlusionTextCoordSet = 0;
    uint8_t m_emissiveTextCoordSet = 0;

    AlphaMode m_alphaMode = AlphaMode::Opaque;
    float m_alphaCutoff = 0.5f;

    ResourceHandle<DescriptorSet> m_descriptorSet;
    static const std::vector<VkDescriptorSetLayoutBinding> ms_bindings;

public:
    void Init();
};

struct Vertex
{
    glm::vec3 m_position;
    glm::vec3 m_normal;
    glm::vec2 m_uvSet0;
    glm::vec2 m_uvSet1;

    static VkVertexInputBindingDescription GetBindingDescription();
    static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();
};

struct Primitive
{
    uint32_t m_firstIndex  = 0;
    uint32_t m_indexCount  = 0;
    uint64_t m_vertexCount = 0;
    bool m_hasIndices = false;
    SharedPtr<Material> m_material;
};

class StaticMeshComponent : public EntityComponent
{
public:
    void SetVertices(std::vector<Vertex> const& vertices);
    void SetIndices(std::vector<uint32_t> const& indices);
    void SetPrimitives(std::vector<Primitive> const& primitives) { m_primitives = primitives; }

    VkBuffer GetVertexBuffer() const { return m_vertexBuffer.GetBuffer(); }
    VkBuffer GetIndexBuffer() const { return m_indexBuffer.GetBuffer(); }
    std::vector<Primitive> const& GetPrimitives() const { return m_primitives; }

protected:
    std::vector<Primitive> m_primitives;
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;

    Buffer m_vertexBuffer;
    Buffer m_indexBuffer;
};
