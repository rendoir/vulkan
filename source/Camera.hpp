#pragma once

#include "Utils.hpp"


class Camera
{
public:
    Camera();

    void SetPosition(glm::vec3 position);
    void SetRotation(glm::vec3 rotation);
    void SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane);
    void SetAspectRatio(float aspectRatio);

    glm::mat4 inline GetView()          const { return m_view; };
    glm::mat4 inline GetProjection()    const { return m_projection; };
    glm::vec3 inline GetPosition()      const { return m_transform.m_position; };
    glm::vec3 inline GetRotation()      const { return m_transform.m_rotation; };
    glm::vec3 inline GetWorldPosition() const { return m_worldPosition; };

private:
    void UpdateViewMatrix();
    void UpdateProjectionMatrix();

private:
    Transform m_transform;

    float m_fov         = glm::radians(45.0f);
    float m_near        = 0.1f;
    float m_far         = 1000.0f;
    float m_aspectRatio = 1.0f;

    glm::mat4 m_view;
    glm::mat4 m_projection;

    glm::vec3 m_worldPosition;
};


class CameraControl
{
public:
    CameraControl(Camera* camera);
    virtual ~CameraControl();

    virtual void HandleInput(GLFWwindow* window) = 0;

protected:
    Camera* m_camera = nullptr;
};


class OrbitControl : public CameraControl
{
public:
    OrbitControl(Camera* camera);
    
    virtual void HandleInput(GLFWwindow* window);

public:
    float m_orbitSensitivity = 0.2f;
    float m_zoomSensitivity  = 0.0025f;

private:
    bool  m_firstMouse;
    float m_lastMouseX;
    float m_lastMouseY;
};
