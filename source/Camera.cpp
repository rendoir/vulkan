#include "Camera.hpp"

Camera::Camera() {
    control = new CameraControl(this);
    updateViewMatrix();
    updateProjectionMatrix();
}

Camera::~Camera() {
    delete control;
}

void Camera::setPosition(glm::vec3 position) {
    this->position = position;
    updateViewMatrix();
}

void Camera::setTarget(glm::vec3 target) {
    this->target = target;
    updateViewMatrix();
}

void Camera::setPerspective(float fov, float aspectRatio, float near, float far) {
    this->fov = fov;
    this->aspectRatio = aspectRatio;
    this->near = near;
    this->far = far;
    updateProjectionMatrix();
}

void Camera::setAspectRatio(float aspectRatio) {
    this->aspectRatio = aspectRatio;
    updateProjectionMatrix();
}

void Camera::updateViewMatrix() {
    view = glm::lookAt(position, target, up);
}

void Camera::updateProjectionMatrix() {
    projection = glm::perspective(fov, aspectRatio, near, far);
    projection[1][1] *= -1;
}


CameraControl::CameraControl(Camera* camera) {
    this->camera = camera;
    firstMouse = true;
    distance = 1.0f;
    pitch = 0.0f;
    yaw = 180.0f;
}

void CameraControl::handleInput(GLFWwindow *window) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    if (firstMouse) {
        lastMouseX = xpos;
        lastMouseY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastMouseX;
    float yoffset = lastMouseY - ypos;

    lastMouseX = xpos;
    lastMouseY = ypos;

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        // TODO - Fix pitch flip
        pitch -= yoffset * orbitSensitivity;
        yaw -= xoffset * orbitSensitivity;
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
        distance += yoffset * zoomSensitivity;
        distance = glm::max(distance, 0.0f);
    }

    float verticalDistance = distance * glm::sin(glm::radians(pitch));
    float horizontalDistance = distance * glm::cos(glm::radians(pitch));

    glm::vec3 position;
    position.x = camera->getTarget().x - horizontalDistance * glm::sin(glm::radians(yaw));
    position.y = camera->getTarget().y + verticalDistance;
    position.z = camera->getTarget().z - horizontalDistance * glm::cos(glm::radians(yaw));
    camera->setPosition(position);
}
