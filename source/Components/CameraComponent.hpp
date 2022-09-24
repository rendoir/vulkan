#pragma once

#include <Components/EntityComponent.hpp>
#include <Resources/Buffer.hpp>
#include <Resources/Descriptor.hpp>
#include <Resources/ResourceComponent.hpp>
#include <Resources/ResourceHandle.hpp>
#include <Systems/ResourceSystem.hpp>
#include <Utilities/Observer.hpp>
#include <Utilities/Singleton.hpp>

class SceneComponent;

class CameraComponent : public EntityComponent
{
public:
    void SetFieldOfVision(float fov);
    void SetNearPlane(float nearPlane); 
    void SetFarPlane(float farPlane);
    void SetAspectRatio(float aspectRatio);
    void SetFollowResolutionAsAspectRatio(bool shouldFollow) { m_shouldFollowResolutionAsAspectRatio = shouldFollow; }

    bool GetFollowResolutionAsAspectRatio() const { return m_shouldFollowResolutionAsAspectRatio; }
    glm::mat4 GetPerspectiveMatrix();
    static glm::mat4 GetViewMatrix(glm::vec3 const& worldPosition, glm::quat const& worldRotation);

private:
    void UpdatePerspectiveMatrix();

protected:
    float m_fov = glm::radians(45.0f);
    float m_near = 0.1f;
    float m_far = 10000.0f;
    float m_aspectRatio = 16.0f/9.0f;
    bool m_shouldFollowResolutionAsAspectRatio = true;

private:
    glm::mat4 m_perspectiveMatrix;
    bool m_isPerspectiveMatrixDirty = true;
};

class OrbitCameraControlComponent : public EntityComponent
{
public:
    glm::vec3 m_centerPosition{ 0.0f };
    float m_radius = 1.0f;
    float m_theta = 0.0f;
    float m_phi = glm::half_pi<float>();
};

class CameraComponentResource : public ComponentResourceInFlight
{
public:
    struct UniformData 
    {
        glm::mat4 m_projection;
        glm::mat4 m_view;
        glm::vec3 m_position;
    };

public:
    CameraComponentResource(CameraComponent const& camera);
    
public:
    ResourceHandleInFlight<Buffer> m_uniformBuffer;

    static const std::vector<VkDescriptorSetLayoutBinding> ms_bindings;
};

class CameraResourceSystem : 
    public ResourceSystem<CameraComponent, CameraComponentResource>,
    public Singleton<CameraResourceSystem>,
    public SwapchainObserver
{
public:
    void Init() override;
    void Update() override;
    void Terminate() override;

    virtual void OnSwapchainRecreated(VkExtent2D const& newExtent) override;

private:
    void UpdateCameraResource(CameraComponentResource& cameraResource, CameraComponent const& camera, SceneComponent const& scene);
};
