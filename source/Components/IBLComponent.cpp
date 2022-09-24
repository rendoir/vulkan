#include <Components/IBLComponent.hpp>

#include <Resources/TextureResource.hpp>
#include <Systems/Renderer.hpp>

const std::vector<VkDescriptorSetLayoutBinding> IBLComponent::ms_bindings = {
    { 0, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }, // BRDFLUT
    { 1, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }, // Irradiance
    { 2, VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1, VK_SHADER_STAGE_FRAGMENT_BIT, nullptr } // Prefilter
};

IBLComponent::IBLComponent()
{
    m_descriptorSet = CreateResourceHandle<DescriptorSet>(IBLComponent::ms_bindings);
}

void IBLComponent::SetBrdflut(TextureResource& brdflutImage)
{
    m_brdflut = brdflutImage.GetSharedPtr();
    VkDescriptorImageInfo descriptorInfo = m_brdflut->GetDescriptorInfo();
    descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.dstSet = m_descriptorSet.GetResource().GetDescriptorSet();
    writeDescriptorSet.dstBinding = 0;
    writeDescriptorSet.pImageInfo = &descriptorInfo;

    Renderer const& renderer = Renderer::GetInstance();
    vkUpdateDescriptorSets(renderer.GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}

void IBLComponent::SetIrradiance(SharedPtr<TextureResource>& irradianceImage)
{
    m_irradianceCube = irradianceImage;

    VkDescriptorImageInfo descriptorInfo = m_irradianceCube->GetDescriptorInfo();
    descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.dstSet = m_descriptorSet.GetResource().GetDescriptorSet();
    writeDescriptorSet.dstBinding = 1;
    writeDescriptorSet.pImageInfo = &descriptorInfo;

    Renderer const& renderer = Renderer::GetInstance();
    vkUpdateDescriptorSets(renderer.GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}

void IBLComponent::SetPrefiltered(SharedPtr<TextureResource>& prefilteredImage)
{
    m_prefilteredCube = prefilteredImage;

    VkDescriptorImageInfo descriptorInfo = m_prefilteredCube->GetDescriptorInfo();
    descriptorInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    VkWriteDescriptorSet writeDescriptorSet = {};
    writeDescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    writeDescriptorSet.descriptorCount = 1;
    writeDescriptorSet.dstSet = m_descriptorSet.GetResource().GetDescriptorSet();
    writeDescriptorSet.dstBinding = 2;
    writeDescriptorSet.pImageInfo = &descriptorInfo;

    Renderer const& renderer = Renderer::GetInstance();
    vkUpdateDescriptorSets(renderer.GetDevice(), 1, &writeDescriptorSet, 0, nullptr);
}
