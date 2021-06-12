#pragma once

#include "Utils.hpp"

struct Texture3D;
class Model;
class Renderer;

class Skybox
{
public:
    Model* m_model = nullptr;
    Texture3D* m_textureCube = nullptr;

    VkPipeline m_pipeline = VK_NULL_HANDLE;
    VkPipelineLayout m_pipelineLayout = VK_NULL_HANDLE;

    VkDescriptorPool m_descriptorPool = VK_NULL_HANDLE;
    VkDescriptorSetLayout m_descriptorSetLayout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSet> m_descriptorSets;

    std::vector<VkBuffer> m_uniformBuffers;
    std::vector<VkDeviceMemory> m_uniformBuffersMemory;

    SkyboxMatrices m_uboMatrices;
    
    struct Vertex
    {
        glm::vec3 m_position;

        static VkVertexInputBindingDescription GetBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 1> GetAttributeDescriptions();
    };

public:
    Skybox(Texture3D* texture);
    ~Skybox();

    void CreateUniformBuffers();
    void CreateDescriptors();
    void CreatePipeline();

    void Draw(int16_t i);

    void UpdateUniformBuffer(int16_t i);
    void OnSwapchainRecreation();

private:
    void Init();
    void Cleanup();
};