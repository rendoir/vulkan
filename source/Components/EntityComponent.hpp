#pragma once

class EntityComponent
{
public:
    inline entt::entity GetEntity() const { return m_entity; }

private:
    inline void SetEntity(entt::entity entity) { m_entity = entity; }
    
protected:
    entt::entity m_entity = entt::null;

friend class EntitySystem;
};
