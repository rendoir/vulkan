#pragma once

#include <Resources/Descriptor.hpp>
#include <Resources/ResourceComponent.hpp>
#include <Utilities/Helpers.hpp>

class TextureResource;

class IBLComponent : public ComponentResource
{
public:
    IBLComponent();

    void SetBrdflut(TextureResource& brdflutImage);
    void SetIrradiance(SharedPtr<TextureResource>& irradianceImage);
    void SetPrefiltered(SharedPtr<TextureResource>& prefilteredImage);

public:
    static const std::vector<VkDescriptorSetLayoutBinding> ms_bindings;

public:
    SharedPtr<TextureResource> m_brdflut;
    SharedPtr<TextureResource> m_irradianceCube;
    SharedPtr<TextureResource> m_prefilteredCube;
};
