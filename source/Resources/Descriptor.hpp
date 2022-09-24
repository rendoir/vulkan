#pragma once

struct DescriptorPoolInstance
{
    VkDescriptorPool m_pool = VK_NULL_HANDLE;
    uint8_t m_allocatedSets = 0;
    static const uint32_t ms_maxSetsPerPool;
};

class DescriptorSetLayout final
{
public:
    DescriptorSetLayout() = default;
    DescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding> const& bindings);
    DescriptorSetLayout(DescriptorSetLayout const& other) = delete;
    DescriptorSetLayout& operator=(DescriptorSetLayout const& other) = delete;
    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept;
    ~DescriptorSetLayout() = default;

    VkDescriptorSetLayout& GetLayout() { return m_layout; }
    VkDescriptorSetLayout const& GetLayout() const { return m_layout; }
    std::vector<VkDescriptorSetLayoutBinding> const& GetBindings() const { return *m_bindings; }

private:
    VkDescriptorSetLayout m_layout = VK_NULL_HANDLE;
    std::vector<VkDescriptorSetLayoutBinding> const* m_bindings = nullptr;
};

class DescriptorSet final
{
public:
    DescriptorSet() = default;
    DescriptorSet(std::vector<VkDescriptorSetLayoutBinding> const& bindings);
    DescriptorSet(DescriptorSet const& other) = delete;
    DescriptorSet& operator=(DescriptorSet const& other) = delete;
    DescriptorSet(DescriptorSet&& other) noexcept;
    DescriptorSet& operator=(DescriptorSet&& other) noexcept;
    ~DescriptorSet();

    VkDescriptorSet& GetDescriptorSet() { return m_descriptorSet; }
    VkDescriptorSet const& GetDescriptorSet() const { return m_descriptorSet; }
    
    DescriptorPoolInstance* GetPoolInstance() { return m_poolInstance; }
    void SetPoolInstance(DescriptorPoolInstance* poolInstance) { m_poolInstance = poolInstance; }

private:
    VkDescriptorSet m_descriptorSet = VK_NULL_HANDLE;
    DescriptorPoolInstance* m_poolInstance = nullptr;
    DescriptorSetLayout m_descriptorLayout;
};
