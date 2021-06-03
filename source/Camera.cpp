#include "Camera.hpp"

Camera::Camera() {
    UpdateViewMatrix();
    UpdateProjectionMatrix();
}

void Camera::SetPosition(glm::vec3 position) {
    m_transform.m_position = position;
    UpdateViewMatrix();
}

void Camera::SetRotation(glm::vec3 rotation) {
    m_transform.m_rotation = rotation;
    UpdateViewMatrix();
}

void Camera::SetPerspective(float fov, float aspectRatio, float nearPlane, float farPlane) {
    this->m_fov = fov;
    this->m_aspectRatio = aspectRatio;
    this->m_near = nearPlane;
    this->m_far = farPlane;
    UpdateProjectionMatrix();
}

void Camera::SetAspectRatio(float aspectRatio) {
    this->m_aspectRatio = aspectRatio;
    UpdateProjectionMatrix();
}

void Camera::UpdateViewMatrix() {
    glm::mat4 rotM = glm::mat4(1.0f);
    glm::mat4 transM;

    rotM = glm::rotate(rotM, glm::radians(m_transform.m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(m_transform.m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(m_transform.m_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    transM = glm::translate(glm::mat4(1.0f), m_transform.m_position * glm::vec3(-1.0f, -1.0f, -1.0f));

    m_view = transM * rotM;
    
    m_worldPosition = glm::inverse(m_view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Assume the camera is on the root object 
}

void Camera::UpdateProjectionMatrix() {
    m_projection = glm::perspective(m_fov, m_aspectRatio, m_near, m_far);
    m_projection[1][1] *= -1;
}


CameraControl::CameraControl(Camera* camera) {
    this->m_camera = camera;
}

CameraControl::~CameraControl() { }

OrbitControl::OrbitControl(Camera* camera) : 
    CameraControl(camera) { }

void OrbitControl::HandleInput(GLFWwindow *window) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (m_firstMouse) {
        m_lastMouseX = xpos;
        m_lastMouseY = ypos;
        m_firstMouse = false;
    }

    float xoffset = xpos - m_lastMouseX;
    float yoffset = m_lastMouseY - ypos;

    m_lastMouseX = xpos;
    m_lastMouseY = ypos;

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        glm::vec3 rotation = m_camera->GetRotation();
        rotation.x -= yoffset * m_orbitSensitivity;
        rotation.y += xoffset * m_orbitSensitivity;
        m_camera->SetRotation(rotation);
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
        float distanceOffset = yoffset * m_zoomSensitivity;
        glm::vec3 position = m_camera->GetPosition() + glm::vec3(0, 0, distanceOffset);
        m_camera->SetPosition(position);
    }
}
