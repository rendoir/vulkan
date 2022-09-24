#pragma once

#include <Systems/EntitySystem.hpp>

template<typename ResourceableComponent, typename ResourceComponent>
class ResourceSystem : public System
{
public:
    virtual void Init() override;
    virtual void Terminate() override;

private:
    void OnResourceableComponentCreated(entt::registry& registry, entt::entity entity);
    void OnResourceableComponentDestroyed(entt::registry& registry, entt::entity entity);
};

template<typename ResourceableComponent, typename ResourceComponent>
void ResourceSystem<ResourceableComponent, ResourceComponent>::Init()
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    entitySystem.AddOnConstructEvent<ResourceableComponent, &ResourceSystem<ResourceableComponent, ResourceComponent>::OnResourceableComponentCreated>(*this);
    entitySystem.AddOnDestroyEvent<ResourceableComponent, &ResourceSystem<ResourceableComponent, ResourceComponent>::OnResourceableComponentDestroyed>(*this);
}

template<typename ResourceableComponent, typename ResourceComponent>
void ResourceSystem<ResourceableComponent, ResourceComponent>::Terminate()
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    entitySystem.RemoveOnConstructEvent<ResourceableComponent, &ResourceSystem<ResourceableComponent, ResourceComponent>::OnResourceableComponentCreated>(*this);
    entitySystem.RemoveOnDestroyEvent<ResourceableComponent, &ResourceSystem<ResourceableComponent, ResourceComponent>::OnResourceableComponentDestroyed>(*this);
}

template<typename ResourceableComponent, typename ResourceComponent>
void ResourceSystem<ResourceableComponent, ResourceComponent>::OnResourceableComponentCreated(entt::registry&, entt::entity entity)
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    auto const& resourceableComponent = entitySystem.GetComponent<const ResourceableComponent>(entity);
    entitySystem.AddComponent<ResourceComponent>(entity, resourceableComponent);
}

template<typename ResourceableComponent, typename ResourceComponent>
void ResourceSystem<ResourceableComponent, ResourceComponent>::OnResourceableComponentDestroyed(entt::registry&, entt::entity entity)
{
    EntitySystem::GetInstance().RemoveComponent<ResourceComponent>(entity);
}

template<typename GlobalResourceComponent>
class GlobalResourceSystem : public System
{
public:
    virtual void Init() override;
    virtual void Terminate() override;
};

template<typename GlobalResourceComponent>
void GlobalResourceSystem<GlobalResourceComponent>::Init()
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    entitySystem.AddComponent<GlobalResourceComponent>(entitySystem.GetGlobalEntity());
}

template<typename GlobalResourceComponent>
void GlobalResourceSystem<GlobalResourceComponent>::Terminate()
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    entitySystem.RemoveComponent<GlobalResourceComponent>(entitySystem.GetGlobalEntity());
}
