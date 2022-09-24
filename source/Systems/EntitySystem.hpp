#pragma once

#include <Systems/System.hpp>
#include <Utilities/Singleton.hpp>

using Entity = entt::entity;

class DirtySystem;

template<typename ViewType>
class View
{
public:
    View(ViewType&& rawView) : m_rawView(std::move(rawView)) {}

    View() = delete;
    View(View const&) = delete;
    View& operator=(View const&) = delete;
    View(View&&) noexcept = default;
    View& operator=(View&&) noexcept = default;

    template <typename Component>
    inline std::enable_if_t<!std::is_const<Component>::value, Component&>
    Get(Entity entity) const;

    template <typename Component>
    inline std::enable_if_t<std::is_const<Component>::value, Component&>
    Get(Entity entity) const;

    inline decltype(auto) begin() const noexcept { return m_rawView.begin(); }
    inline decltype(auto) end() const noexcept { return m_rawView.end(); }
    inline decltype(auto) rbegin() const noexcept { return m_rawView.rbegin(); }
    inline decltype(auto) rend() const noexcept { return m_rawView.rend(); }
    inline decltype(auto) front() const { return m_rawView.front(); }
    inline decltype(auto) back() const { return m_rawView.back(); }

private:
    ViewType m_rawView;
};

class EntitySystem : public System, public Singleton<EntitySystem>
{
public:
    virtual void Init() override;
    virtual void Terminate() override;

    inline entt::entity CreateEntity();
    inline void DestroyEntity(entt::entity entity);

    template<typename... Component>
    inline bool HasComponent(entt::entity entity);
    
    template<class Component>
    inline std::enable_if_t<!std::is_const<Component>::value, Component&>
    GetComponent(entt::entity entity);

    template<class Component>
    inline std::enable_if_t<std::is_const<Component>::value, Component&>
    GetComponent(entt::entity entity);

    template<class Component>
    inline std::enable_if_t<!std::is_const<Component>::value, Component*>
    TryGetComponent(entt::entity entity);

    template<class Component>
    inline std::enable_if_t<std::is_const<Component>::value, Component*>
    TryGetComponent(entt::entity entity);
    
    template<class Component, class... Args>
    inline std::enable_if_t<!std::is_const<Component>::value, Component&>
    AddComponent(entt::entity entity, Args&&... args);

    template<class Component, class... Args>
    inline std::enable_if_t<std::is_const<Component>::value, Component&>
    AddComponent(entt::entity entity, Args&&... args);

    template<class Component, class... Args>
    inline std::enable_if_t<!std::is_const<Component>::value, Component&>
    AddOrReplaceComponent(entt::entity entity, Args&&... args);

    template<class Component, class... Args>
    inline std::enable_if_t<std::is_const<Component>::value, Component&>
    AddOrReplaceComponent(entt::entity entity, Args&&... args);

    template<class Component, class... Args>
    inline std::enable_if_t<!std::is_const<Component>::value, Component&>
    GetOrAddComponent(entt::entity entity, Args&&... args);

    template<class Component, class... Args>
    inline std::enable_if_t<std::is_const<Component>::value, Component&>
    GetOrAddComponent(entt::entity entity, Args&&... args);
    
    template<class Component>
    inline void RemoveComponent(entt::entity entity);

    template<typename Component, auto Handler, typename ValueOrInstance>
    inline void AddOnConstructEvent(ValueOrInstance& valueOrInstance);

    template<typename Component, auto Handler, typename ValueOrInstance>
    inline void RemoveOnConstructEvent(ValueOrInstance& valueOrInstance);

    template<typename Component, auto Handler, typename ValueOrInstance>
    inline void AddOnDestroyEvent(ValueOrInstance& valueOrInstance);

    template<typename Component, auto Handler, typename ValueOrInstance>
    inline void RemoveOnDestroyEvent(ValueOrInstance& valueOrInstance);

    template<typename... Component, typename... Exclude>
    inline decltype(auto) GetView(entt::exclude_t<Exclude...> = {});

    inline entt::entity GetGlobalEntity() const { return m_globalEntity; }

    static inline bool IsEntityValid(entt::entity entity);

private:
    entt::registry m_registry;
    entt::entity m_globalEntity = entt::null;

    friend class Singleton<EntitySystem>;
};

#include <Systems/DirtySystem.hpp>

template <typename ViewType>
template <typename Component>
inline std::enable_if_t<!std::is_const<Component>::value, Component&>
View<ViewType>::Get(Entity entity) const
{ 
    DirtySystem::GetInstance().MarkComponentAsDirty<Component>(entity);
    return m_rawView.template get<Component>(entity);
}

template <typename ViewType>
template <typename Component>
inline std::enable_if_t<std::is_const<Component>::value, Component&>
View<ViewType>::Get(Entity entity) const
{ 
    return m_rawView.template get<Component>(entity);
}

inline entt::entity EntitySystem::CreateEntity()
{
    return m_registry.create();
}

inline void EntitySystem::DestroyEntity(entt::entity entity)
{
    m_registry.destroy(entity);
}

template<typename... Component>
inline bool EntitySystem::HasComponent(entt::entity entity)
{
    return m_registry.all_of<Component...>(entity);
}

template<class Component>
inline std::enable_if_t<!std::is_const<Component>::value, Component&>
EntitySystem::GetComponent(entt::entity entity)
{
    DirtySystem::GetInstance().MarkComponentAsDirty<Component>(entity);
    return m_registry.get<Component>(entity);
}

template<class Component>
inline std::enable_if_t<std::is_const<Component>::value, Component&>
EntitySystem::GetComponent(entt::entity entity)
{
    return m_registry.get<Component>(entity);
}

template<class Component>
inline std::enable_if_t<!std::is_const<Component>::value, Component*>
EntitySystem::TryGetComponent(entt::entity entity)
{
    Component* const component = m_registry.try_get<Component>(entity);
    if (component)
    {
        DirtySystem::GetInstance().MarkComponentAsDirty<Component>(entity);
    }

    return component;
}

template<class Component>
inline std::enable_if_t<std::is_const<Component>::value, Component*>
EntitySystem::TryGetComponent(entt::entity entity)
{
    return m_registry.try_get<Component>(entity);
}

template<class Component, class... Args>
inline std::enable_if_t<!std::is_const<Component>::value, Component&>
EntitySystem::AddComponent(entt::entity entity, Args&&... args)
{
    Component& component = m_registry.emplace<Component>(entity, std::forward<Args>(args)...);
    component.SetEntity(entity);
    DirtySystem::GetInstance().MarkComponentAsDirty<Component>(entity);
    return component;
}

template<class Component, class... Args>
inline std::enable_if_t<std::is_const<Component>::value, Component&>
EntitySystem::AddComponent(entt::entity entity, Args&&... args)
{
    Component& component = m_registry.emplace<Component>(entity, std::forward<Args>(args)...);
    component.SetEntity(entity);
    return component;
}

template<class Component, class... Args>
inline std::enable_if_t<!std::is_const<Component>::value, Component&>
EntitySystem::AddOrReplaceComponent(entt::entity entity, Args&&... args)
{
    Component& component = m_registry.emplace_or_replace<Component>(entity, std::forward<Args>(args)...);
    component.SetEntity(entity);
    DirtySystem::GetInstance().MarkComponentAsDirty<Component>(entity);
    return component;
}

template<class Component, class... Args>
inline std::enable_if_t<std::is_const<Component>::value, Component&>
EntitySystem::AddOrReplaceComponent(entt::entity entity, Args&&... args)
{
    Component& component = m_registry.emplace_or_replace<Component>(entity, std::forward<Args>(args)...);
    component.SetEntity(entity);
    return component;
}

template<class Component, class... Args>
inline std::enable_if_t<!std::is_const<Component>::value, Component&>
EntitySystem::GetOrAddComponent(entt::entity entity, Args&&... args)
{
    Component& component = m_registry.get_or_emplace<Component>(entity, std::forward<Args>(args)...);
    component.SetEntity(entity);
    DirtySystem::GetInstance().MarkComponentAsDirty<Component>(entity);
    return component;
}

template<class Component, class... Args>
inline std::enable_if_t<std::is_const<Component>::value, Component&>
EntitySystem::GetOrAddComponent(entt::entity entity, Args&&... args)
{
    Component& component = m_registry.get_or_emplace<Component>(entity, std::forward<Args>(args)...);
    component.SetEntity(entity);
    return component;
}

template<class Component>
inline void EntitySystem::RemoveComponent(entt::entity entity)
{
    m_registry.erase<Component>(entity);
}

template<typename Component, auto Handler, typename ValueOrInstance>
inline void EntitySystem::AddOnConstructEvent(ValueOrInstance& valueOrInstance)
{
    m_registry.on_construct<Component>().template connect<Handler>(valueOrInstance);
}

template<typename Component, auto Handler, typename ValueOrInstance>
inline void EntitySystem::RemoveOnConstructEvent(ValueOrInstance& valueOrInstance)
{
    m_registry.on_construct<Component>().template disconnect<Handler>(valueOrInstance);
}

template<typename Component, auto Handler, typename ValueOrInstance>
inline void EntitySystem::AddOnDestroyEvent(ValueOrInstance& valueOrInstance)
{
    m_registry.on_destroy<Component>().template connect<Handler>(valueOrInstance);
}

template<typename Component, auto Handler, typename ValueOrInstance>
inline void EntitySystem::RemoveOnDestroyEvent(ValueOrInstance& valueOrInstance)
{
    m_registry.on_destroy<Component>().template disconnect<Handler>(valueOrInstance);
}

template<typename... Component, typename... Exclude>
inline decltype(auto) EntitySystem::GetView(entt::exclude_t<Exclude...> excludes)
{
    return View(
        m_registry.view<Component...>(std::forward<entt::exclude_t<Exclude...>>(excludes))
    );
}

/*static*/ inline bool EntitySystem::IsEntityValid(entt::entity entity)
{
    return entity != entt::null; 
}
