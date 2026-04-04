#include <Components/LightComponent.hpp>

#include <Components/SceneComponent.hpp>
#include <Systems/EntitySystem.hpp>
#include <Systems/Renderer.hpp>

const std::vector<VkDescriptorSetLayoutBinding> LightComponentGlobalResource::ms_bindings = {
    { 0, VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }
};

LightComponentGlobalResource::LightComponentGlobalResource()
{
    BufferInfo bufferCreationInfo;
    bufferCreationInfo.m_size = sizeof(UniformData);
    bufferCreationInfo.m_usage = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    bufferCreationInfo.m_memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    m_uniformBuffer = CreateResourceInFlight<Buffer>(bufferCreationInfo);
    m_descriptorSet = CreateResourceInFlight<DescriptorSet>(LightComponentGlobalResource::ms_bindings);

    Renderer const& renderer = Renderer::GetInstance();

    uint8_t i = 0;
    for (DescriptorSet& set : m_descriptorSet)
    {
        VkDescriptorBufferInfo bufferInfo = {};
        bufferInfo.buffer = m_uniformBuffer[i].GetBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformData);

        VkWriteDescriptorSet writeDescriptorSet = {};
        writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.dstSet = set.GetDescriptorSet();
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(renderer.GetDevice(), 1, &writeDescriptorSet, 0, nullptr);

        i++;
    }
}

void LightGlobalResourceSystem::Update()
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    auto const& view = entitySystem.GetView<SceneComponent const, LightComponent const>();
    
    LightComponentGlobalResource::UniformData data;

    data.m_nrLights = 0;
    for (entt::entity entity : view)
    {
        if (data.m_nrLights >= LightComponentGlobalResource::ms_maxNumberOfLights)
            break;

        SceneComponent const& scene = view.Get<SceneComponent const>(entity);
        LightComponent const& light = view.Get<LightComponent const>(entity);

        data.m_lights[data.m_nrLights].m_color = glm::vec4(light.m_color, 1.0f);
        data.m_lights[data.m_nrLights].m_worldPosition = glm::vec4(scene.GetWorldTranslation(), 1.0f);
    
        ++data.m_nrLights;
    }

    LightComponentGlobalResource& globalResource = entitySystem.GetComponent<LightComponentGlobalResource>(entitySystem.GetGlobalEntity());

    Buffer& buffer = globalResource.m_uniformBuffer.GetResource();
    LightComponentGlobalResource::UniformData* mappedMemory = static_cast<LightComponentGlobalResource::UniformData*>(buffer.MapMemory());
    memcpy(&mappedMemory->m_nrLights, &data.m_nrLights, sizeof(data.m_nrLights));
    memcpy(&mappedMemory->m_lights, &data.m_lights, sizeof(LightComponentGlobalResource::UniformData::LightUniformData) * data.m_nrLights);
    buffer.UnmapMemory();
}
