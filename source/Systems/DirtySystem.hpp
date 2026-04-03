#pragma once

#include <Components/EntityComponent.hpp>
#include <Systems/Renderer.hpp>
#include <Systems/System.hpp>
#include <Utilities/Helpers.hpp>
#include <Utilities/Singleton.hpp>

class EntitySystem;

class DirtyComponent : public EntityComponent
{
public:
    int8_t m_dirtyCounter = 0;
};

template<class Component>
struct DirtyComponentMap;

#define DECLARE_DIRTY_COMPONENT(Component) \
    class Component; \
    class Dirty##Component : public DirtyComponent {}; \
    template<> struct DirtyComponentMap<Component> { typedef Dirty##Component Type; };
#define DECLARE_DIRTY_COMPONENT_FOR_EACH(I, Data, Component) DECLARE_DIRTY_COMPONENT(Component)
#define DECLARE_DIRTY_COMPONENTS(...) BOOST_PP_SEQ_FOR_EACH(DECLARE_DIRTY_COMPONENT_FOR_EACH, , BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))
    
#define DIRTIABLE_COMPONENTS \
    CameraComponent, \
    SceneComponent

#define GLOBAL_DIRTIABLE_COMPONENTS \
    LightComponent

DECLARE_DIRTY_COMPONENTS(DIRTIABLE_COMPONENTS);
DECLARE_DIRTY_COMPONENTS(GLOBAL_DIRTIABLE_COMPONENTS);

using DirtiableComponentsTypeList = TypeList<DIRTIABLE_COMPONENTS>;
using GlobalDirtiableComponentsTypeList = TypeList<GLOBAL_DIRTIABLE_COMPONENTS>;

class DirtySystem : public System, public Singleton<DirtySystem>
{
public:
    template<typename Component>
    void MarkComponentAsDirty(entt::entity);

    virtual void Update() override;

private:
    template<typename... DirtiableComponents>
    inline void UpdateDirtyComponents(TypeList<DirtiableComponents...>&&);

    template<typename DirtiableComponent>
    void UpdateDirtyComponent();
};

#include <Systems/EntitySystem.hpp>

template<typename Component>
void DirtySystem::MarkComponentAsDirty(entt::entity entity)
{
    if constexpr (TypelistContainsType<Component>(DirtiableComponentsTypeList{}))
    {
        auto& dirtyComponent = EntitySystem::GetInstance().GetOrAddComponent<typename DirtyComponentMap<Component>::Type>(entity);
        dirtyComponent.m_dirtyCounter = Renderer::RenderSettings::m_maxFramesInFlight;
    }
    else if constexpr (TypelistContainsType<Component>(GlobalDirtiableComponentsTypeList{}))
    {
        EntitySystem& entitySystem = EntitySystem::GetInstance();
        auto& dirtyComponent = entitySystem.GetOrAddComponent<typename DirtyComponentMap<Component>::Type>(entitySystem.GetGlobalEntity());
        dirtyComponent.m_dirtyCounter = Renderer::RenderSettings::m_maxFramesInFlight;
    }
    // else: Component is not dirtiable, do nothing
}

template<typename... DirtiableComponents>
inline void DirtySystem::UpdateDirtyComponents(TypeList<DirtiableComponents...>&&)
{
    (UpdateDirtyComponent<DirtiableComponents>(), ...);
}

template<typename DirtiableComponent>
void DirtySystem::UpdateDirtyComponent()
{
    using DirtyComponentName = typename DirtyComponentMap<DirtiableComponent>::Type;

    EntitySystem& entitySystem = EntitySystem::GetInstance();
    auto const& view = entitySystem.GetView<DirtyComponentName>();

    for (entt::entity entity : view)
    {
        DirtyComponentName& dirtyComponent = view.template Get<DirtyComponentName>(entity);
        dirtyComponent.m_dirtyCounter--;

        if (dirtyComponent.m_dirtyCounter == 0)
        {
            // No longer dirty
            entitySystem.RemoveComponent<DirtyComponentName>(entity);
        }
    }
}
