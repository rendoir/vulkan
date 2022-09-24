#pragma once

#include <Resources/ResourceHandle.hpp>
#include <Resources/Descriptor.hpp>
#include <Systems/EntitySystem.hpp>

class ComponentResourceBase : public EntityComponent
{
};

class ComponentResource : public ComponentResourceBase
{
public:
    ResourceHandle<DescriptorSet> m_descriptorSet;
};

class ComponentResourceInFlight : public ComponentResourceBase
{
public:
    ResourceHandleInFlight<DescriptorSet> m_descriptorSet;
};
