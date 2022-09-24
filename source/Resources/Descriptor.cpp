#include <Resources/Descriptor.hpp>

#include <Systems/ResourceManager.hpp>

const uint32_t DescriptorPoolInstance::ms_maxSetsPerPool = USHRT_MAX;

/// DescriptorSetLayout
DescriptorSetLayout::DescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> const& bindings)
{
    m_layout = ResourceManager::GetInstance().GetDescriptorLayout(bindings);
    m_bindings = &bindings;
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
{
    m_layout = other.m_layout;
    m_bindings = other.m_bindings;

    other.m_layout = VK_NULL_HANDLE;
    other.m_bindings = nullptr;
}

DescriptorSetLayout& DescriptorSetLayout::operator=(DescriptorSetLayout&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    m_layout = other.m_layout;
    m_bindings = other.m_bindings;

    other.m_layout = VK_NULL_HANDLE;
    other.m_bindings = nullptr;

    return *this;
}

/// DescriptorSet
DescriptorSet::DescriptorSet(std::vector<VkDescriptorSetLayoutBinding> const& bindings)
{
    m_descriptorLayout = DescriptorSetLayout(bindings);
    ResourceManager& resourceManager = ResourceManager::GetInstance();
    resourceManager.AllocateDescriptorSet(m_descriptorLayout, *this);
}

DescriptorSet::DescriptorSet(DescriptorSet&& other) noexcept
{
    m_descriptorLayout = std::move(other.m_descriptorLayout);
    m_descriptorSet = other.m_descriptorSet;
    m_poolInstance = other.m_poolInstance;

    other.m_descriptorSet = VK_NULL_HANDLE;
    other.m_poolInstance = nullptr;
}

DescriptorSet& DescriptorSet::operator=(DescriptorSet&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    m_descriptorLayout = std::move(other.m_descriptorLayout);
    m_descriptorSet = other.m_descriptorSet;
    m_poolInstance = other.m_poolInstance;

    other.m_descriptorSet = VK_NULL_HANDLE;
    other.m_poolInstance = nullptr;

    return *this;
}

DescriptorSet::~DescriptorSet()
{
    if (m_descriptorSet != VK_NULL_HANDLE)
    {
        ResourceManager& resourceManager = ResourceManager::GetInstance();
        resourceManager.FreeDescriptorSet(*this);
    }
}
