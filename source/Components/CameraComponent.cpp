#include <Components/CameraComponent.hpp>

#include <Components/SceneComponent.hpp>
#include <Systems/DirtySystem.hpp>
#include <Systems/EntitySystem.hpp>
#include <Systems/Renderer.hpp>

void CameraComponent::SetFieldOfVision(float fov)
{
    m_fov = fov;
    m_isPerspectiveMatrixDirty = true;
}

void CameraComponent::SetNearPlane(float nearPlane)
{
    m_near = nearPlane;
    m_isPerspectiveMatrixDirty = true;
}

void CameraComponent::SetFarPlane(float farPlane)
{
    m_far = farPlane;
    m_isPerspectiveMatrixDirty = true;
}

void CameraComponent::SetAspectRatio(float aspectRatio)
{
    m_aspectRatio = aspectRatio;
    m_isPerspectiveMatrixDirty = true;
}

void CameraComponent::UpdatePerspectiveMatrix()
{
    m_perspectiveMatrix = glm::perspective(m_fov, m_aspectRatio, m_near, m_far);
    m_perspectiveMatrix[1][1] *= -1;

    m_isPerspectiveMatrixDirty = false;
}

glm::mat4 CameraComponent::GetPerspectiveMatrix()
{
    if (m_isPerspectiveMatrixDirty)
    {
        UpdatePerspectiveMatrix();
    }
    
    return m_perspectiveMatrix;
}

/*static*/ glm::mat4 CameraComponent::GetViewMatrix(glm::vec3 const& worldPosition, glm::quat const& worldRotation)
{
    glm::mat4 const translationMatrix = glm::translate(glm::mat4(1.0f), worldPosition);
    glm::mat4 const rotationMatrix = glm::toMat4(worldRotation);

    return glm::inverse(translationMatrix * rotationMatrix);
}

const std::vector<VkDescriptorSetLayoutBinding> CameraComponentResource::ms_bindings = {
    { 0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 1, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT, nullptr }
};

CameraComponentResource::CameraComponentResource(CameraComponent const&)
{
    BufferInfo bufferCreationInfo;
    bufferCreationInfo.m_size = sizeof(UniformData);
    bufferCreationInfo.m_usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    bufferCreationInfo.m_memoryUsage = VMA_MEMORY_USAGE_CPU_TO_GPU;
    m_uniformBuffer = CreateResourceHandleInFlight<Buffer>(bufferCreationInfo);
    m_descriptorSet = CreateResourceHandleInFlight<DescriptorSet>(CameraComponentResource::ms_bindings);

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

void CameraResourceSystem::Init()
{
    ResourceSystem<CameraComponent, CameraComponentResource>::Init();
    Renderer::GetInstance().AddSwapchainObserver(this);
}

void CameraResourceSystem::Terminate()
{
    Renderer::GetInstance().RemoveSwapchainObserver(this);
    ResourceSystem<CameraComponent, CameraComponentResource>::Terminate();
}

void CameraResourceSystem::Update()
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    auto const& cameraDirtyView = entitySystem.GetView<CameraComponentResource, const CameraComponent, const SceneComponent, const DirtyCameraComponent>();
    for (entt::entity entity : cameraDirtyView)
    {
        UpdateCameraResource(
            cameraDirtyView.Get<CameraComponentResource>(entity),
            cameraDirtyView.Get<const CameraComponent>(entity),
            cameraDirtyView.Get<const SceneComponent>(entity)
        );
    }
    
    auto const& sceneDirtyView = entitySystem.GetView<CameraComponentResource, const CameraComponent, const SceneComponent, const DirtySceneComponent>(entt::exclude<DirtyCameraComponent>);
    for (entt::entity entity : sceneDirtyView)
    {
        UpdateCameraResource(
            sceneDirtyView.Get<CameraComponentResource>(entity),
            sceneDirtyView.Get<const CameraComponent>(entity),
            sceneDirtyView.Get<const SceneComponent>(entity)
        );
    }
}

void CameraResourceSystem::UpdateCameraResource(CameraComponentResource& cameraResource, CameraComponent const& camera, SceneComponent const& scene)
{
    SceneComponent& sceneRef = const_cast<SceneComponent&>(scene);
    CameraComponent& cameraRef = const_cast<CameraComponent&>(camera);

    CameraComponentResource::UniformData data;
    data.m_projection = cameraRef.GetPerspectiveMatrix();
    data.m_position = sceneRef.GetWorldTranslation();
    data.m_view = cameraRef.GetViewMatrix(data.m_position, sceneRef.GetWorldRotation());

    Buffer& buffer = cameraResource.m_uniformBuffer.GetResource();
    buffer.CopyDataToBuffer(&data, sizeof(CameraComponentResource::UniformData));
}

void CameraResourceSystem::OnSwapchainRecreated(VkExtent2D const& newExtent)
{
    float const newAspectRatio = (float)newExtent.width / (float)newExtent.height;
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    auto const& cameraView = entitySystem.GetView<CameraComponent>();
    for (entt::entity entity : cameraView)
    {
        CameraComponent& camera = cameraView.Get<CameraComponent>(entity);
        if (camera.GetFollowResolutionAsAspectRatio())
        {
            camera.SetAspectRatio(newAspectRatio);
        }
    }
}
