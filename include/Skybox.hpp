#pragma once
#include <vulkan/vulkan.h>
#include <vector>
#include <array>

#include "Utils.hpp"

struct Texture3D;
class Model;
class Renderer;

class Skybox {
public:
    Model* model;
    Texture3D* textureCube;
    Renderer* renderer;

    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;

    VkDescriptorPool descriptorPool;
    VkDescriptorSetLayout descriptorSetLayout;
    std::vector<VkDescriptorSet> descriptorSets;

    std::vector<VkBuffer> uniformBuffers;
    std::vector<VkDeviceMemory> uniformBuffersMemory;

    SkyboxMatrices uboMatrices;
    
    struct Vertex {
        glm::vec3 position;

        static VkVertexInputBindingDescription getBindingDescription();
        static std::array<VkVertexInputAttributeDescription, 1> getAttributeDescriptions();
    };

public:
    Skybox(Renderer* renderer, Texture3D* texture);
    ~Skybox();

    void createUniformBuffers();
    void createDescriptors();
    void createPipeline();

    void draw(size_t i);

    void updateUniformBuffer(size_t i);
    void onSwapchainRecreation();

private:
    void init();
    void cleanup();
};