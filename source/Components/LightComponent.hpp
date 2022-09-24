#pragma once

#include <Components/EntityComponent.hpp>
#include <Resources/Descriptor.hpp>
#include <Resources/Buffer.hpp>
#include <Resources/ResourceComponent.hpp>
#include <Resources/ResourceHandle.hpp>
#include <Systems/ResourceSystem.hpp>
#include <Utilities/Singleton.hpp>

class LightComponent : public EntityComponent
{
public:
    glm::vec3 m_color{ 1.0f };
};

class LightComponentGlobalResource : public ComponentResourceInFlight
{
public:
    static constexpr uint32_t ms_maxNumberOfLights = 128;

    struct UniformData 
    {
        struct LightUniformData
        {
            glm::vec4 m_color;
            glm::vec4 m_worldPosition;
        };

        LightUniformData m_lights[ms_maxNumberOfLights];
        uint32_t m_nrLights = 0;
    };

public:
    LightComponentGlobalResource();
    
public:
    ResourceHandleInFlight<Buffer> m_uniformBuffer;

    static const std::vector<VkDescriptorSetLayoutBinding> ms_bindings;
};

class LightGlobalResourceSystem
    : public GlobalResourceSystem<LightComponentGlobalResource>, public Singleton<LightGlobalResourceSystem>
{
public:
    void Init() override;
    void Update() override;
    void Terminate() override;

private:
    template<typename ViewType>
    void UpdateResourceData(ViewType const& view);

    void MarkLightComponentAsDirty(entt::registry& registry, entt::entity entity);
    void MarkLightComponentAsDirtyInternal();
};
