#pragma once

#include <Resources/Descriptor.hpp>
#include <Resources/TextureResource.hpp>
#include <Systems/System.hpp>
#include <Utilities/Helpers.hpp>
#include <Utilities/Singleton.hpp>

class ResourceManager : public System, public Singleton<ResourceManager>
{
using DescriptorLayoutBindings = std::vector<VkDescriptorSetLayoutBinding>;

public:
    virtual void Init() override;
    virtual void Terminate() override;

    VkDescriptorSetLayout GetDescriptorLayout(DescriptorLayoutBindings const& descriptorLayoutBindings);
    void AllocateDescriptorSet(DescriptorSetLayout const& layout, DescriptorSet& set);
    void FreeDescriptorSet(DescriptorSet& set);

    VkShaderModule GetShaderModule(std::string const& fileName);

    TextureResource const& GetEmptyTexture() const { return *m_emptyTexture; }

private:
    VkDescriptorSetLayout CreateDescriptorSetLayout(DescriptorLayoutBindings const& descriptorLayoutBindings);
    DescriptorPoolInstance& CreateDescriptorPool(DescriptorSetLayout const& layout);
    DescriptorPoolInstance& GetDescriptorPool(DescriptorSetLayout const& layout);

    VkShaderModule CreateShaderModule(std::string const& fileName);

    void CreateEmptyTexture();

private:
    std::map<DescriptorLayoutBindings, VkDescriptorSetLayout> m_descriptorLayoutMap;
    std::multimap<VkDescriptorSetLayout, DescriptorPoolInstance> m_descriptorLayoutPoolMap;

    std::map<std::string, VkShaderModule> m_shaderModuleMap;

    UniquePtr<TextureResource> m_emptyTexture = nullptr;

    friend class Singleton<ResourceManager>;
};
