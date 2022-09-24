#include <Systems/ResourceManager.hpp>

#include <Resources/ImageResource.hpp>
#include <Systems/Renderer.hpp>

void ResourceManager::Init()
{
    CreateEmptyTexture();
}

void ResourceManager::Terminate()
{
    Renderer& renderer = Renderer::GetInstance();

    // Destroy all descriptor set layouts
    for (const auto& [_, layout] : m_descriptorLayoutMap)
    {
        vkDestroyDescriptorSetLayout(renderer.GetDevice(), layout, nullptr);
    }
    m_descriptorLayoutMap.clear();

    // Destroy all descriptor pools
    for (const auto& [_, poolInstance] : m_descriptorLayoutPoolMap)
    {
        Assert(poolInstance.m_allocatedSets == 0, "Requesting destruction of Descriptor Pool with allocated sets");
        vkDestroyDescriptorPool(renderer.GetDevice(), poolInstance.m_pool, nullptr);
    }
    m_descriptorLayoutPoolMap.clear();

    // Destroy all shader modules
    for (const auto& [_, shaderModule] : m_shaderModuleMap)
    {
        vkDestroyShaderModule(renderer.GetDevice(), shaderModule, nullptr);
    }
    m_shaderModuleMap.clear();

    m_emptyTexture->Destroy();
}

VkDescriptorSetLayout ResourceManager::GetDescriptorLayout(DescriptorLayoutBindings const& descriptorLayoutBindings)
{
    // Try to get the descriptor layout from cache
    auto foundIt = m_descriptorLayoutMap.find(descriptorLayoutBindings);
    if (foundIt != m_descriptorLayoutMap.end())
    {
        return foundIt->second;
    }

    // Create descriptor layout and pool
    VkDescriptorSetLayout const layout = CreateDescriptorSetLayout(descriptorLayoutBindings);
    CreateDescriptorPool(descriptorLayoutBindings);

    return layout;
}

DescriptorPoolInstance& ResourceManager::GetDescriptorPool(DescriptorSetLayout const& layout)
{
    // Get the descriptor pool from cache
    auto foundIts = m_descriptorLayoutPoolMap.equal_range(layout.GetLayout());
    for (auto it = foundIts.first; it != foundIts.second; ++it)
    {
        if (it->second.m_allocatedSets < DescriptorPoolInstance::ms_maxSetsPerPool)
        {
            return it->second;
        }
    }

    return CreateDescriptorPool(layout.GetBindings());
}

VkDescriptorSetLayout ResourceManager::CreateDescriptorSetLayout(DescriptorLayoutBindings const& descriptorLayoutBindings)
{
    Renderer& renderer = Renderer::GetInstance();

    VkDescriptorSetLayout layout = VK_NULL_HANDLE;

    VkDescriptorSetLayoutCreateInfo layoutInfo = {};
    layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    layoutInfo.pBindings = descriptorLayoutBindings.data();
    layoutInfo.bindingCount = descriptorLayoutBindings.size();
    if (vkCreateDescriptorSetLayout(renderer.GetDevice(), &layoutInfo, nullptr, &layout) != VK_SUCCESS)
    {
        ThrowError("Failed to create descriptor set layout.");
    }

    auto emplaceResult = m_descriptorLayoutMap.emplace(std::make_pair(descriptorLayoutBindings, layout));
    if (!emplaceResult.second)
    {
        ThrowError("Failed to emplace descriptor set layout.");
    }

    return layout;
}

DescriptorPoolInstance& ResourceManager::CreateDescriptorPool(DescriptorSetLayout const& layout)
{
    // Loop layout bindings to find how many descriptors of each type are needed per layout and multiply by the max sets per pool
    std::map<VkDescriptorType, uint8_t> descriptorCounts;
    for (VkDescriptorSetLayoutBinding const& binding : layout.GetBindings())
    {
        auto foundIt = descriptorCounts.find(binding.descriptorType);
        if (foundIt != descriptorCounts.end())
        {
            foundIt->second++;
        }
        else
        {
            descriptorCounts[binding.descriptorType] = 1;
        }
    }
    
    std::vector<VkDescriptorPoolSize> poolSizes;
    for (const auto& [descriptorType, count] : descriptorCounts)
    {
        poolSizes.push_back({ descriptorType, count * DescriptorPoolInstance::ms_maxSetsPerPool });
    }

    VkDescriptorPoolCreateInfo poolInfo = {};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.poolSizeCount = poolSizes.size();
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = DescriptorPoolInstance::ms_maxSetsPerPool;

    Renderer& renderer = Renderer::GetInstance();
    DescriptorPoolInstance poolInstance;
    if (vkCreateDescriptorPool(renderer.GetDevice(), &poolInfo, nullptr, &poolInstance.m_pool) != VK_SUCCESS)
    {
        ThrowError("Failed to create descriptor pool.");
    }

    return m_descriptorLayoutPoolMap.emplace(std::make_pair(layout.GetLayout(), poolInstance))->second;
}

void ResourceManager::AllocateDescriptorSet(DescriptorSetLayout const& layout, DescriptorSet& set)
{
    DescriptorPoolInstance& poolInstance = GetDescriptorPool(layout);

    VkDescriptorSetAllocateInfo descriptorSetAllocInfo = {};
    descriptorSetAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    descriptorSetAllocInfo.descriptorPool = poolInstance.m_pool;
    descriptorSetAllocInfo.pSetLayouts = &layout.GetLayout();
    descriptorSetAllocInfo.descriptorSetCount = 1;

    Renderer& renderer = Renderer::GetInstance();
    if (vkAllocateDescriptorSets(renderer.GetDevice(), &descriptorSetAllocInfo, &set.GetDescriptorSet()) != VK_SUCCESS)
    {
        ThrowError("Failed to allocate descriptor set.");
    }

    poolInstance.m_allocatedSets++;

    set.SetPoolInstance(&poolInstance);
}

void ResourceManager::FreeDescriptorSet(DescriptorSet& set)
{   
    Renderer& renderer = Renderer::GetInstance();
    DescriptorPoolInstance* poolInstance = set.GetPoolInstance();
    if (vkFreeDescriptorSets(renderer.GetDevice(), poolInstance->m_pool, 1, &set.GetDescriptorSet()) != VK_SUCCESS)
    {
        ThrowError("Failed to free descriptor set.");
    }

    poolInstance->m_allocatedSets--;
}

VkShaderModule ResourceManager::GetShaderModule(std::string const& fileName)
{
    // Try to get the shader module from cache
    auto foundIt = m_shaderModuleMap.find(fileName);
    if (foundIt != m_shaderModuleMap.end())
    {
        return foundIt->second;
    }

    // Create shader module
    return CreateShaderModule(fileName);
}

VkShaderModule ResourceManager::CreateShaderModule(std::string const& fileName)
{
    std::string const& shaderCode = ReadShaderFile(fileName);

    VkShaderModuleCreateInfo shaderModuleInfo = {};
    shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfo.codeSize = shaderCode.size();
    shaderModuleInfo.pCode = reinterpret_cast<uint32_t const*>(shaderCode.data());

    VkShaderModule shaderModule;
    if (vkCreateShaderModule(Renderer::GetInstance().GetDevice(), &shaderModuleInfo, nullptr, &shaderModule) != VK_SUCCESS)
    {
        ThrowError("Failed to create shader module.");
    }

    m_shaderModuleMap[fileName] = shaderModule;

    return shaderModule;
}

void ResourceManager::CreateEmptyTexture()
{
    static constexpr uint8_t s_emptyData[] = { 0, 0, 0, 0 };
    static constexpr TextureCreationInfo s_emptyTextureInfo
    {
        /* m_imageCreateInfo */
        {
            /*m_width*/                 1.0f,
            /*m_height*/                1.0f,
            /*m_sizeType*/              SizeType::Absolute,
            /*m_layers*/                1,
            /*m_mipLevels*/             1,
            /*m_format*/                VK_FORMAT_R8G8B8A8_UNORM,
            /*m_sampleCount*/           VK_SAMPLE_COUNT_1_BIT
        },
        /*m_channels*/              4,
        /*m_bytesPerChannel*/       1,
        /*m_samplerInfo*/           TextureSampler(),
        /*m_data*/                  s_emptyData
    };
    
    m_emptyTexture = std::make_unique<TextureResource>(s_emptyTextureInfo);
}
