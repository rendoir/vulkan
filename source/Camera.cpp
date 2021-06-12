#include "Camera.hpp"

Camera::Camera()
{
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::SetPosition(glm::vec3 const position)
{
    m_transform.m_position = position;
    UpdateViewMatrix();
}

void Camera::SetRotation(glm::vec3 const rotation)
{
    m_transform.m_rotation = rotation;
    UpdateViewMatrix();
}

void Camera::SetPerspective(float const fov, float const aspectRatio, float const nearPlane, float const farPlane)
{
    m_fov = fov;
    m_aspectRatio = aspectRatio;
    m_near = nearPlane;
    m_far = farPlane;
    UpdateProjectionMatrix();
}

void Camera::SetAspectRatio(float const aspectRatio)
{
    m_aspectRatio = aspectRatio;
    UpdateProjectionMatrix();
}

void Camera::UpdateViewMatrix()
{
    glm::mat4 rotationMatrix = glm::mat4(1.0f);
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_transform.m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_transform.m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotationMatrix = glm::rotate(rotationMatrix, glm::radians(m_transform.m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), m_transform.m_position * glm::vec3(-1.0f, -1.0f, -1.0f));

    m_view = translationMatrix * rotationMatrix;
    
    m_worldPosition = glm::inverse(m_view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Assume the camera is on the root object 
}

void Camera::UpdateProjectionMatrix()
{
    m_projection = glm::perspective(m_fov, m_aspectRatio, m_near, m_far);
    m_projection[1][1] *= -1;
}


CameraControl::CameraControl(Camera* camera)
{
    m_camera = camera;
}

OrbitControl::OrbitControl(Camera* camera) : 
    CameraControl(camera) { }

void OrbitControl::HandleInput(GLFWwindow* window)
{
    double xPosition, yPosition;
    glfwGetCursorPos(window, &xPosition, &yPosition);

    if (m_firstMouse)
    {
        m_lastMouseX = xPosition;
        m_lastMouseY = yPosition;
        m_firstMouse = false;
    }

    float const xOffset = xPosition - m_lastMouseX;
    float const yOffset = m_lastMouseY - yPosition;

    m_lastMouseX = xPosition;
    m_lastMouseY = yPosition;

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT))
    {
        glm::vec3 rotation = m_camera->GetRotation();
        rotation.x -= yOffset * m_orbitSensitivity;
        rotation.y += xOffset * m_orbitSensitivity;
        m_camera->SetRotation(rotation);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT))
    {
        float const distanceOffset = yOffset * m_zoomSensitivity;
        glm::vec3 position = m_camera->GetPosition() + glm::vec3(0, 0, distanceOffset);
        m_camera->SetPosition(position);
    }
}
