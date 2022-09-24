#include <Components/SceneComponent.hpp>

#include <Systems/DirtySystem.hpp>
#include <Systems/EntitySystem.hpp>
#include <Systems/Renderer.hpp>

SceneComponent::~SceneComponent()
{
    Detach();

    for (entt::entity child : m_children)
    {
        SceneComponent& childScene = EntitySystem::GetInstance().GetComponent<SceneComponent>(child);
        childScene.Detach();
    }
}

bool SceneComponent::AttachTo(entt::entity parent)
{
    if (!EntitySystem::IsEntityValid(parent) || parent == m_parent)
    {
        return false;
    }

    if (IsDescendent(parent))
    {
        return false; // Trying to attach to one of our descendents
    }

    if (EntitySystem::IsEntityValid(m_parent))
    {
        Detach();
    }
   
    SceneComponent& parentScene = EntitySystem::GetInstance().GetComponent<SceneComponent>(parent);
    parentScene.m_children.push_back(m_entity);
    m_parent = parent;

    SetWorldTranslation(GetLocalTranslation());
    SetWorldRotation(GetLocalRotation());
    SetWorldScale(GetLocalScale());

    return true;
}

bool SceneComponent::Detach()
{
    if (!EntitySystem::IsEntityValid(m_parent))
    {
        return false;
    }

    SetLocalTranslation(GetWorldTranslation());
    SetLocalRotation(GetWorldRotation());
    SetLocalScale(GetWorldScale());

    SceneComponent& parentScene = EntitySystem::GetInstance().GetComponent<SceneComponent>(m_parent);
    auto const childIt = std::find(parentScene.m_children.begin(), parentScene.m_children.end(), m_entity);
    if (childIt != parentScene.m_children.end())
    {
        parentScene.m_children.erase(childIt);
    }
    m_parent = entt::null;

    return true;
}

void SceneComponent::InvalidateWorldTransformRecursively()
{
    if (m_isWorldTransformDirty)
    {
        return; // Already dirty from another call
    }

    // Invalidate our world transform and our children's recursively
    m_isWorldTransformDirty = true;

    for (entt::entity child : m_children)
    {
        SceneComponent& childScene = EntitySystem::GetInstance().GetComponent<SceneComponent>(child);
        childScene.InvalidateWorldTransformRecursively();
    }
}

void SceneComponent::SetLocalTranslation(glm::vec3 const& translation) 
{
    m_localTransform.SetTranslation(translation);
    InvalidateWorldTransformRecursively();
}

void SceneComponent::SetLocalScale(glm::vec3 const& scale) 
{
    m_localTransform.SetScale(scale);
    InvalidateWorldTransformRecursively();
}

void SceneComponent::SetLocalRotation(glm::quat const& rotation) 
{
    m_localTransform.SetRotation(rotation);
    InvalidateWorldTransformRecursively();
}

void SceneComponent::SetLocalEulerRotation(glm::vec3 const& eulerRotation) 
{
    m_localTransform.SetEulerRotation(eulerRotation);
    InvalidateWorldTransformRecursively();
}

glm::mat4 const& SceneComponent::GetWorldMatrix()
{
    if (m_isWorldTransformDirty)
    {
        CalculateWorldTransform();
    }

    return m_worldTransform.GetMatrix();
}

glm::vec3 const& SceneComponent::GetWorldTranslation()
{
    if (m_isWorldTransformDirty)
    {
        CalculateWorldTransform();
    }

    return m_worldTransform.GetTranslation();
}

glm::quat const& SceneComponent::GetWorldRotation()
{
    if (m_isWorldTransformDirty)
    {
        CalculateWorldTransform();
    }

    return m_worldTransform.GetRotation();
}

glm::vec3 SceneComponent::GetWorldEulerRotation()
{
    if (m_isWorldTransformDirty)
    {
        CalculateWorldTransform();
    }

    return m_worldTransform.GetEulerRotation();
}

glm::vec3 const& SceneComponent::GetWorldScale()
{
    if (m_isWorldTransformDirty)
    {
        CalculateWorldTransform();
    }

    return m_worldTransform.GetScale();
}

void SceneComponent::SetWorldTranslation(glm::vec3 const& translation)
{
    if (EntitySystem::IsEntityValid(m_parent))
    {
        SceneComponent& parentScene = EntitySystem::GetInstance().GetComponent<SceneComponent>(m_parent);
        glm::mat4 const& parentWorldMatrix = parentScene.GetWorldMatrix();
        m_localTransform.SetTranslation(glm::inverse(parentWorldMatrix) * glm::vec4(translation, 1.0f));
    }
    else
    {
        m_localTransform.SetTranslation(translation);
    }

    InvalidateWorldTransformRecursively();
}

void SceneComponent::SetWorldScale(glm::vec3 const& scale)
{
    if (EntitySystem::IsEntityValid(m_parent))
    {
        SceneComponent& parentScene = EntitySystem::GetInstance().GetComponent<SceneComponent>(m_parent);
        m_localTransform.SetScale(scale / parentScene.GetWorldScale());
    }
    else
    {
        m_localTransform.SetScale(scale);
    }

    InvalidateWorldTransformRecursively();
}

void SceneComponent::SetWorldRotation(glm::quat const& rotation)
{
    if (EntitySystem::IsEntityValid(m_parent))
    {
        SceneComponent& parentScene = EntitySystem::GetInstance().GetComponent<SceneComponent>(m_parent);
        m_localTransform.SetRotation(glm::inverse(parentScene.GetWorldRotation()) * rotation);
    }
    else
    {
        m_localTransform.SetRotation(rotation);
    }

    InvalidateWorldTransformRecursively();
}

void SceneComponent::SetWorldEulerRotation(glm::vec3 const& eulerRotation)
{
    SetWorldRotation(glm::quat(eulerRotation));
}

void SceneComponent::CalculateWorldTransform()
{
    if (EntitySystem::IsEntityValid(m_parent))
    {
        SceneComponent& parentScene = EntitySystem::GetInstance().GetComponent<SceneComponent>(m_parent);
        m_worldTransform.SetTranslation(parentScene.GetWorldMatrix() * glm::vec4(m_localTransform.GetTranslation(), 1.0f));
        m_worldTransform.SetRotation(parentScene.GetWorldRotation() * m_localTransform.GetRotation());
        m_worldTransform.SetScale(parentScene.GetWorldScale() * m_localTransform.GetScale());
    }
    else
    {
        m_worldTransform = m_localTransform;
    }

    m_isWorldTransformDirty = false;
}

bool SceneComponent::IsDescendent(entt::entity const entity) const
{
    for (entt::entity child : m_children)
    {
        if (child == entity) 
        {
            return true;
        }
        
        SceneComponent const& childScene = EntitySystem::GetInstance().GetComponent<const SceneComponent>(child);
        if (childScene.IsDescendent(entity))
        {
            return true;
        }
    }

    return false;
}

SceneComponent::Transform::Transform(glm::vec3 const& translation, glm::quat const& rotation, glm::vec3 const& scale)
    : m_translation(translation)
    , m_rotation(rotation)
    , m_scale(scale)
{
}

glm::vec3 SceneComponent::Transform::GetEulerRotation() const
{
    // Returns pitch, yaw and roll
    return glm::eulerAngles(m_rotation);
}

glm::mat4 const& SceneComponent::Transform::GetMatrix()
{
    if (m_isMatrixDirty)
    {
        CalculateMatrix();
    }
    
    return m_matrix;
}

void SceneComponent::Transform::CalculateMatrix()
{
    glm::mat4 const translationMatrix = glm::translate(glm::mat4(1.0f), m_translation);
    glm::mat4 const rotationMatrix = glm::toMat4(m_rotation);
    glm::mat4 const scaleMatrix = glm::scale(glm::mat4(1.0f), m_scale);

    m_matrix = translationMatrix * rotationMatrix * scaleMatrix;

    m_isMatrixDirty = false;
}

void SceneComponent::Transform::SetTranslation(glm::vec3 const& translation)
{
    m_translation = translation;
    m_isMatrixDirty = true;
}

void SceneComponent::Transform::SetRotation(glm::quat const& rotation)
{
    m_rotation = rotation;
    m_isMatrixDirty = true;
}

void SceneComponent::Transform::SetScale(glm::vec3 const& scale)
{
    m_scale = scale;
    m_isMatrixDirty = true;
}

void SceneComponent::Transform::SetEulerRotation(glm::vec3 const& eulerRotation)
{
    m_rotation = glm::quat(eulerRotation);
    m_isMatrixDirty = true;
}

void SceneComponent::Transform::Translate(glm::vec3 const& translation)
{
    m_translation += translation;
    m_isMatrixDirty = true;
}

void SceneComponent::Transform::Rotate(glm::vec3 const& eulerRotation)
{
    // Applies euler angles in XYZ order
    Rotate(glm::quat(eulerRotation));
}

void SceneComponent::Transform::Rotate(glm::quat const& rotation)
{
    m_rotation = m_rotation * rotation;
    m_isMatrixDirty = true;
}

void SceneComponent::Transform::Scale(glm::vec3 const& scale)
{
    m_scale *= scale;
    m_isMatrixDirty = true;
}

const std::vector<VkDescriptorSetLayoutBinding> SceneComponentResource::ms_bindings = {
    { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT, nullptr }
};

SceneComponentResource::SceneComponentResource(SceneComponent const&)
{
    BufferInfo bufferCreationInfo;
    bufferCreationInfo.m_size = sizeof(UniformData);
    bufferCreationInfo.m_usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreationInfo.m_memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    m_uniformBuffer = CreateResourceHandleInFlight<Buffer>(bufferCreationInfo);
    m_descriptorSet = CreateResourceHandleInFlight<DescriptorSet>(SceneComponentResource::ms_bindings);

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
        writeDescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        writeDescriptorSet.descriptorCount = 1;
        writeDescriptorSet.dstSet = set.GetDescriptorSet();
        writeDescriptorSet.dstBinding = 0;
        writeDescriptorSet.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(renderer.GetDevice(), 1, &writeDescriptorSet, 0, nullptr);

        i++;
    }
}

void SceneResourceSystem::Update()
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    auto const& sceneDirtyView = entitySystem.GetView<SceneComponentResource, const SceneComponent, const DirtySceneComponent>();
    for (entt::entity entity : sceneDirtyView)
    {
        SceneComponentResource& resourceComponent = sceneDirtyView.Get<SceneComponentResource>(entity);
        SceneComponent& sceneComponent = const_cast<SceneComponent&>(sceneDirtyView.Get<const SceneComponent>(entity));

        SceneComponentResource::UniformData data;
        data.m_worldMatrix = const_cast<SceneComponent&>(sceneComponent).GetWorldMatrix();

        Buffer& buffer = resourceComponent.m_uniformBuffer.GetResource();
        buffer.CopyDataToBuffer(&data, sizeof(SceneComponentResource::UniformData));
    }
}
