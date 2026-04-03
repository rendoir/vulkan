#include <Components/SkyboxComponent.hpp>

#include <Resources/TextureResource.hpp>
#include <Resources/Descriptor.hpp>
#include <Resources/ResourceInFlight.hpp>
#include <Systems/Renderer.hpp>

SkyboxComponent::SkyboxComponent(SharedPtr<TextureResource>& texture)
    : m_cubeTexture(texture)
{
}

const std::vector<VkDescriptorSetLayoutBinding> SkyboxComponentResource::ms_bindings = {
    { 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }
};

SkyboxComponentResource::SkyboxComponentResource(SkyboxComponent const& skybox)
    : ComponentResource(SkyboxComponentResource::ms_bindings)
{
    VkDescriptorImageInfo const& descriptorInfo = skybox.GetTextureCube()->GetDescriptorInfo();

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.dstSet = m_descriptorSet.GetDescriptorSet();
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.pImageInfo = &descriptorInfo;

    Renderer const& renderer = Renderer::GetInstance();
    vkUpdateDescriptorSets(renderer.GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}
