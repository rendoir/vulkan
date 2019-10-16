#pragma once

#include <vulkan/vulkan.h>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <array>
#include <string>
#include <vector>

namespace tinygltf {
    struct Image;
    struct Model;
    struct Node;
}

class Renderer;

struct TextureSampler {
    VkFilter magFilter;
    VkFilter minFilter;
    VkSamplerAddressMode addressModeU;
    VkSamplerAddressMode addressModeV;
    VkSamplerAddressMode addressModeW;

    const static TextureSampler defaultSampler;
};

struct Texture {
    Renderer *renderer;

    VkImage image;
    VkImageLayout imageLayout;
    VkDeviceMemory deviceMemory;
    VkImageView view;
    uint32_t width, height;
    uint32_t mipLevels;
    uint32_t layerCount;
    VkDescriptorImageInfo descriptor;
    VkSampler sampler;
    
    void fromglTfImage(tinygltf::Image &gltfimage, TextureSampler textureSampler, Renderer *renderer);
    void fromFile(std::string filename, Renderer *renderer);
    void createTextureImage(unsigned char* imageData, VkDeviceSize imageSize);
    void createTextureImageView();
    void createTextureSampler(TextureSampler textureSampler);
    void destroy();
};

struct Texture3D {
    Renderer *renderer;

    VkImage image;
    VkImageLayout imageLayout;
    VkDeviceMemory deviceMemory;
    VkImageView view;
    uint32_t width, height;
    uint32_t mipLevels;
    uint32_t layerCount;
    VkDescriptorImageInfo descriptor;
    VkSampler sampler;
    
    void fromFolder(std::string folderName, Renderer *renderer);
    void createTextureImage(float* imageData[], VkDeviceSize imageSize);
    void createTextureImageView();
    void createTextureSampler(TextureSampler textureSampler);
    void destroy();
};

struct Material {
    enum AlphaMode { 
        ALPHAMODE_OPAQUE,
        ALPHAMODE_MASK,
        ALPHAMODE_BLEND
    };
    AlphaMode alphaMode = ALPHAMODE_OPAQUE;
    float alphaCutoff = 1.0f;
    
    float metallicFactor = 1.0f;
    float roughnessFactor = 1.0f;
    glm::vec4 baseColorFactor = glm::vec4(1.0f);
    glm::vec4 emissiveFactor = glm::vec4(1.0f);
    
    Texture *baseColorTexture;
    Texture *metallicRoughnessTexture;
    Texture *normalTexture;
    Texture *occlusionTexture;
    Texture *emissiveTexture;

    uint8_t baseColorTextCoordSet = 0;
    uint8_t metallicRoughnessTextCoordSet = 0;
    uint8_t normalTextCoordSet = 0;
    uint8_t occlusionTextCoordSet = 0;
    uint8_t emissiveTextCoordSet = 0;

    VkDescriptorSet descriptorSet = VK_NULL_HANDLE;
};

struct BoundingBox {
    glm::vec3 min;
    glm::vec3 max;
    bool valid = false;

    BoundingBox();
    BoundingBox(glm::vec3 min, glm::vec3 max);
    BoundingBox getAABB(glm::mat4 m);
};

struct Primitive {
    uint32_t firstIndex;
    uint32_t indexCount;
    uint32_t vertexCount;
    Material &material;
    bool hasIndices;
    BoundingBox bb;

    Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material &material);
    void setBoundingBox(glm::vec3 min, glm::vec3 max);
};

struct Mesh {
    Renderer *renderer;

    std::vector<Primitive*> primitives;

    BoundingBox bb;
    BoundingBox aabb;

    // TODO - Should be per frame
    struct UniformBuffer {
        VkBuffer buffer;
        VkDeviceMemory memory;
        VkDescriptorBufferInfo descriptor;
        VkDescriptorSet descriptorSet;
        void *mapped;
    } uniformBuffer;

    struct UniformBlock {
        glm::mat4 matrix;
    } uniformBlock;

    Mesh(Renderer *renderer, glm::mat4 matrix);
    ~Mesh();

    void setBoundingBox(glm::vec3 min, glm::vec3 max);
};

struct Node {
    Node *parent;
    uint32_t index;
    std::vector<Node*> children;
    glm::mat4 matrix;
    std::string name;
    Mesh *mesh;
    glm::vec3 translation{};
    glm::vec3 scale{ 1.0f };
    glm::quat rotation{};
    BoundingBox bvh;
    BoundingBox aabb;

    glm::mat4 localMatrix();
    glm::mat4 getMatrix();
    void update();
    ~Node();
};

struct Model {
    Renderer *renderer;

    struct Vertex {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec2 uv0;
        glm::vec2 uv1;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 4> getAttributeDescriptions();
    };

    struct Vertices {
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory;
    } vertices;

    struct Indices {
        int count;
        VkBuffer buffer = VK_NULL_HANDLE;
        VkDeviceMemory memory;
    } indices;

    glm::mat4 aabb;

    std::vector<Node*> nodes;
    std::vector<Node*> linearNodes;

    std::vector<Texture> textures;
    std::vector<TextureSampler> textureSamplers;
    std::vector<Material> materials;

    struct Dimensions {
        glm::vec3 min = glm::vec3(FLT_MAX);
        glm::vec3 max = glm::vec3(-FLT_MAX);
    } dimensions;

    void loadFromFile(std::string filename, Renderer *renderer, float scale = 1.0f);
    void destroy();

    void loadNode(Node *parent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer, float globalscale);
    void loadTextures(tinygltf::Model &gltfModel, Renderer *renderer);
    void loadMaterials(tinygltf::Model &gltfModel);
    void loadTextureSamplers(tinygltf::Model &gltfModel);

    void drawNode(Node *node, VkCommandBuffer commandBuffer);
    void draw(VkCommandBuffer commandBuffer);

    void calculateBoundingBox(Node *node, Node *parent);
    void getSceneDimensions();
    VkSamplerAddressMode getVkWrapMode(int32_t wrapMode);
    VkFilter getVkFilterMode(int32_t filterMode);

    Node* findNode(Node *parent, uint32_t index);
    Node* nodeFromIndex(uint32_t index);

    void createVertexBuffer(std::vector<Vertex> vertices);
    void createIndexBuffer(std::vector<uint32_t> indices);
};
