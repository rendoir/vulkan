#pragma once

#include <Components/EntityComponent.hpp>
#include <Resources/ResourceInFlight.hpp>
#include <Resources/Descriptor.hpp>

class ComponentResource : public EntityComponent
{
public:
    explicit ComponentResource(std::vector<VkDescriptorSetLayoutBinding> const& bindings)
        : m_descriptorSet(bindings) {}

    const DescriptorSet& GetDescriptorSet() const { return m_descriptorSet; }

protected:
    DescriptorSet m_descriptorSet;
};

class ComponentResourceInFlight : public EntityComponent
{
public:
    const DescriptorSet& GetDescriptorSetInFlight() const { return m_descriptorSet.GetResource(); }

protected:
    ResourceInFlight<DescriptorSet> m_descriptorSet;
};
