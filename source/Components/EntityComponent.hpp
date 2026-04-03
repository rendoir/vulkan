#pragma once

class EntityComponent
{
public:
    entt::entity GetEntity() const { return m_entity; }

private:
    void SetEntity(entt::entity entity) { m_entity = entity; }
    
protected:
    entt::entity m_entity = entt::null;

friend class EntitySystem;
};
