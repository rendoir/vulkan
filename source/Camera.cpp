#include "Camera.hpp"

Camera::Camera() {
    updateViewMatrix();
    updateProjectionMatrix();
}

void Camera::setPosition(glm::vec3 position) {
    transform.position = position;
    updateViewMatrix();
}

void Camera::setRotation(glm::vec3 rotation) {
    transform.rotation = rotation;
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
    glm::mat4 rotM = glm::mat4(1.0f);
    glm::mat4 transM;

    rotM = glm::rotate(rotM, glm::radians(transform.rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(transform.rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    rotM = glm::rotate(rotM, glm::radians(transform.rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    transM = glm::translate(glm::mat4(1.0f), transform.position * glm::vec3(-1.0f, -1.0f, -1.0f));

    view = transM * rotM;
    
    worldPosition = glm::inverse(view) * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f); // Assume the camera is on the root object 
}

void Camera::updateProjectionMatrix() {
    projection = glm::perspective(fov, aspectRatio, near, far);
    projection[1][1] *= -1;
}


CameraControl::CameraControl(Camera* camera) {
    this->camera = camera;
}

CameraControl::~CameraControl() { }

OrbitControl::OrbitControl(Camera* camera) : 
    CameraControl(camera) { }

void OrbitControl::handleInput(GLFWwindow *window) {
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
        glm::vec3 rotation = camera->getRotation();
        rotation.x -= yoffset * orbitSensitivity;
        rotation.y += xoffset * orbitSensitivity;
        camera->setRotation(rotation);
    }

    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
        float distanceOffset = yoffset * zoomSensitivity;
        glm::vec3 position = camera->getPosition() + glm::vec3(0, 0, distanceOffset);
        camera->setPosition(position);
    }
}
