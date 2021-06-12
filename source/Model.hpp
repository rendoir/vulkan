#pragma once

class Renderer;

struct TextureSampler
{
    VkFilter m_magFilter = VK_FILTER_LINEAR;
    VkFilter m_minFilter = VK_FILTER_LINEAR;
    VkSamplerAddressMode m_addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode m_addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
    VkSamplerAddressMode m_addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
};

struct Texture2D
{
    VkImage        m_image        = VK_NULL_HANDLE;
    VkImageView    m_view         = VK_NULL_HANDLE;
    VkSampler      m_sampler      = VK_NULL_HANDLE;
    VkDeviceMemory m_deviceMemory = VK_NULL_HANDLE;
    
    VkImageLayout m_imageLayout;
    VkDescriptorImageInfo m_descriptor;

    int16_t m_width;
    int16_t m_height;
    int16_t m_mipLevels;
    
    void FromglTfImage(tinygltf::Image const& gltfimage, TextureSampler const& textureSampler);
    void FromFile(std::string const filename);
    void CreateTextureImage(uint8_t const* const imageData, VkDeviceSize const& imageSize);
    void CreateTextureImageView();
    void CreateTextureSampler(TextureSampler const& textureSampler);
    void Destroy();
};

struct Texture3D
{
    VkImage        m_image        = VK_NULL_HANDLE;
    VkImageView    m_view         = VK_NULL_HANDLE;
    VkSampler      m_sampler      = VK_NULL_HANDLE;
    VkDeviceMemory m_deviceMemory = VK_NULL_HANDLE;
    
    VkDeviceSize  m_faceSize;
    VkImageLayout m_imageLayout;
    VkDescriptorImageInfo m_descriptor;

    int16_t m_width;
    int16_t m_height;
    int16_t m_mipLevels;
    
    void FromFolder(std::string const folderName);
    void CreateTextureImage(float const* const imageData[]);
    void CreateTextureImageView();
    void CreateTextureSampler();
    void Destroy();
};

struct Material
{
    enum AlphaMode
    { 
        ALPHAMODE_OPAQUE,
        ALPHAMODE_MASK,
        ALPHAMODE_BLEND
    };
    
    float m_metallicFactor  = 1.0f;
    float m_roughnessFactor = 1.0f;
    glm::vec4 m_baseColorFactor = glm::vec4(1.0f);
    glm::vec4 m_emissiveFactor  = glm::vec4(1.0f);
    
    Texture2D* m_baseColorTexture         = nullptr;
    Texture2D* m_metallicRoughnessTexture = nullptr;
    Texture2D* m_normalTexture            = nullptr;
    Texture2D* m_occlusionTexture         = nullptr;
    Texture2D* m_emissiveTexture          = nullptr;

    uint8_t m_baseColorTextCoordSet         = 0;
    uint8_t m_metallicRoughnessTextCoordSet = 0;
    uint8_t m_normalTextCoordSet            = 0;
    uint8_t m_occlusionTextCoordSet         = 0;
    uint8_t m_emissiveTextCoordSet          = 0;

    AlphaMode m_alphaMode = ALPHAMODE_OPAQUE;
    float m_alphaCutoff = 1.0f;

    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
};

struct BoundingBox
{
    glm::vec3 m_min;
    glm::vec3 m_max;
    bool m_valid = false;

    BoundingBox() {}
    BoundingBox(glm::vec3 const& min, glm::vec3 const& max) : m_min(min), m_max(max) {}
    BoundingBox FromMatrix(glm::mat4 const& matrix);
};

struct Primitive
{
    uint32_t m_firstIndex  = 0;
    uint32_t m_indexCount  = 0;
    uint32_t m_vertexCount = 0;
    bool m_hasIndices = false;
    Material* m_material;
    BoundingBox m_boundingBox;

    Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material* material);
    
    void SetBoundingBox(glm::vec3 const& min, glm::vec3 const& max);
};

struct Mesh
{
    BoundingBox m_boundingBox;
    BoundingBox m_boundingBox2;

    // TODO - Should be per frame
    struct UniformBuffer
    {
        VkBuffer        m_buffer        = VK_NULL_HANDLE;
        VkDeviceMemory  m_memory        = VK_NULL_HANDLE;
        VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
        VkDescriptorBufferInfo m_descriptor;
        void* m_mapped;
    } m_uniformBuffer;

    struct UniformBlock
    {
        glm::mat4 m_matrix;
    } m_uniformBlock;

    std::vector<Primitive*> m_primitives;

    Mesh(glm::mat4 const& matrix);
    ~Mesh();

    void SetBoundingBox(glm::vec3 const& min, glm::vec3 const& max);
};

struct Node
{
    Node* m_parent;
    Mesh* m_mesh;
    uint32_t m_index;

    glm::vec3 m_translation = glm::vec3(0.0f);
    glm::vec3 m_scale       = glm::vec3(1.0f);
    glm::quat m_rotation    = glm::quat(0.0f, 0.0f, 0.0f, 1.0f);
    glm::mat4 m_matrix;

    BoundingBox m_boundingBox;
    BoundingBox m_boundingBox2; // TODO: Remove

    std::vector<Node*> m_children;

    glm::mat4 GetLocalMatrix();
    glm::mat4 GetWorldMatrix();
    void Update();

    ~Node();
};

struct Model
{
    struct Vertex
    {
        glm::vec3 m_position;
        glm::vec3 m_normal;
        glm::vec2 m_uvSet0;
        glm::vec2 m_uvSet1;

        static VkVertexInputBindingDescription GetBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 4> GetAttributeDescriptions();
    };

    struct Vertices
    {
        VkBuffer       m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;
    } m_vertices;

    struct Indices
    {
        uint32_t m_count;

        VkBuffer       m_buffer = VK_NULL_HANDLE;
        VkDeviceMemory m_memory = VK_NULL_HANDLE;
    } m_indices;

    glm::mat4 m_boundingBox;

    std::vector<Node*> m_nodes;
    std::vector<Node*> m_linearNodes;

    std::vector<Texture2D> m_textures;
    std::vector<TextureSampler> m_textureSamplers;
    std::vector<Material> m_materials;

    struct Dimensions
    {
        glm::vec3 m_min = glm::vec3(FLT_MAX);
        glm::vec3 m_max = glm::vec3(-FLT_MAX);
    } m_dimensions;

    void LoadFromFile(std::string const filename);
    void Destroy();

    void LoadNode(Node* parent, tinygltf::Node const& node, uint32_t const nodeIndex, tinygltf::Model const& model, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer);
    void LoadTextures(tinygltf::Model const& gltfModel);
    void LoadMaterials(tinygltf::Model& gltfModel);
    void LoadTextureSamplers(tinygltf::Model const& gltfModel);

    void DrawNode(Node const* node, VkCommandBuffer commandBuffer);
    void Draw(VkCommandBuffer commandBuffer);

    void CalculateBoundingBox(Node* node, Node const* parent);
    void GetSceneDimensions();
    VkSamplerAddressMode GetVkWrapMode(int32_t const wrapMode);
    VkFilter GetVkFilterMode(int32_t const filterMode);

    void CreateVertexBuffer(std::vector<Vertex> const& vertices);
    void CreateIndexBuffer(std::vector<uint32_t> const& indices);
};
