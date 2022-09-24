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
    template<
        typename Component,
        std::enable_if_t<!TypelistContainsType<Component>(DirtiableComponentsTypeList{}) && !TypelistContainsType<Component>(GlobalDirtiableComponentsTypeList{}), bool> = true
    >
    inline void MarkComponentAsDirty(entt::entity) { /*Component is not dirtiable*/ }

    template<
        typename Component,
        std::enable_if_t<TypelistContainsType<Component>(DirtiableComponentsTypeList{}), bool> = true
    >
    void MarkComponentAsDirty(entt::entity entity);

    template<
        typename Component,
        std::enable_if_t<TypelistContainsType<Component>(GlobalDirtiableComponentsTypeList{}), bool> = true
    >
    void MarkComponentAsDirty(entt::entity entity);

    virtual void Update() override;

private:
    template<typename... DirtiableComponents>
    inline void UpdateDirtyComponents(TypeList<DirtiableComponents...>&&);

    template<typename DirtiableComponent>
    void UpdateDirtyComponent();
};

#include <Systems/EntitySystem.hpp>

template<
    typename Component,
    std::enable_if_t<TypelistContainsType<Component>(DirtiableComponentsTypeList{}), bool>
>
void DirtySystem::MarkComponentAsDirty(entt::entity entity)
{
    // Component is dirtiable
    auto& dirtyComponent = EntitySystem::GetInstance().GetOrAddComponent<typename DirtyComponentMap<Component>::Type>(entity);
    dirtyComponent.m_dirtyCounter = Renderer::RenderSettings::m_maxFramesInFlight;
}

template<
    typename Component,
    std::enable_if_t<TypelistContainsType<Component>(GlobalDirtiableComponentsTypeList{}), bool>
>
void DirtySystem::MarkComponentAsDirty(entt::entity)
{
    // Component is globally dirtiable
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    auto& dirtyComponent = entitySystem.GetOrAddComponent<typename DirtyComponentMap<Component>::Type>(entitySystem.GetGlobalEntity());
    dirtyComponent.m_dirtyCounter = Renderer::RenderSettings::m_maxFramesInFlight;
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
