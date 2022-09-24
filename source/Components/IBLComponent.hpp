#pragma once

#include <Components/EntityComponent.hpp>
#include <Resources/Descriptor.hpp>
#include <Resources/ResourceHandle.hpp>
#include <Utilities/Helpers.hpp>

class TextureResource;

class IBLComponent : public EntityComponent
{
public:
    IBLComponent();

    void SetBrdflut(TextureResource& brdflutImage);
    void SetIrradiance(SharedPtr<TextureResource>& irradianceImage);
    void SetPrefiltered(SharedPtr<TextureResource>& prefilteredImage);
    DescriptorSet const& GetDescriptorSet() const { return m_descriptorSet.GetResource(); } 

public:
    static const std::vector<VkDescriptorSetLayoutBinding> ms_bindings;

public:
    ResourceHandle<DescriptorSet> m_descriptorSet;
    SharedPtr<TextureResource> m_brdflut;
    SharedPtr<TextureResource> m_irradianceCube;
    SharedPtr<TextureResource> m_prefilteredCube;
};
