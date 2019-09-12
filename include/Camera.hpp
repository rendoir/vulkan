#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>


class CameraControl;

class Camera 
{
private:
    glm::vec3 position = glm::vec3(0.0f, 0.0f, 1.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);

    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    float fov = glm::radians(45.0f);
    float aspectRatio = 1.0f;
    float near = 0.1f;
    float far = 1000.0f;

    glm::mat4 view;
    glm::mat4 projection;

private:
    void updateViewMatrix();
    void updateProjectionMatrix();

public:
    CameraControl *control;

public:
    Camera();
    ~Camera();
    void setPosition(glm::vec3 position);
    void setTarget(glm::vec3 target);
    void setPerspective(float fov, float aspectRatio, float near, float far);
    void setAspectRatio(float aspectRatio);

    glm::mat4 inline getView() { return view; };
    glm::mat4 inline getProjection() { return projection; };
    glm::vec3 inline getPosition() { return position; };
    glm::vec3 inline getTarget() { return target; };
};


class CameraControl {
public:
    float orbitSensitivity = 0.2f;
    float zoomSensitivity = 0.0025f;

private:
    Camera *camera;
    
    bool firstMouse;
    float lastMouseX;
    float lastMouseY;

    float distance;
    float pitch;
    float yaw;

public:
    CameraControl(Camera *camera);
    void handleInput(GLFWwindow *window);
};