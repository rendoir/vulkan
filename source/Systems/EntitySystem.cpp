#include <Systems/EntitySystem.hpp>

void EntitySystem::Init()
{
    m_globalEntity = CreateEntity();
}

void EntitySystem::Terminate()
{
    m_registry.clear();
    m_globalEntity = entt::null;
}
