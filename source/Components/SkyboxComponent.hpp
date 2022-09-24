#pragma once

#include <Components/EntityComponent.hpp>
#include <Resources/ResourceComponent.hpp>
#include <Systems/ResourceSystem.hpp>
#include <Utilities/Helpers.hpp>
#include <Utilities/Singleton.hpp>

class TextureResource;

class SkyboxComponent : public EntityComponent
{
public:
    SkyboxComponent(SharedPtr<TextureResource>& texture);
    virtual ~SkyboxComponent() = default;

    SharedPtr<TextureResource> const& GetTextureCube() const { return m_cubeTexture; }

private:
    SharedPtr<TextureResource> m_cubeTexture;
};

class SkyboxComponentResource : public ComponentResource
{
public:
    SkyboxComponentResource(SkyboxComponent const& skybox);
    
public:
    static const std::vector<VkDescriptorSetLayoutBinding> ms_bindings;

private:
    WeakPtr<TextureResource> m_cubeTexture;
};

class SkyboxResourceSystem : public ResourceSystem<SkyboxComponent, SkyboxComponentResource>, public Singleton<SkyboxResourceSystem>
{
};
