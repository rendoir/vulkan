#pragma once

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GLFW/glfw3.h>

#include "Utils.hpp"

class CameraControl;


enum CameraType {
    FIRST_PERSON,
    THIRD_PERSON
};


class Camera 
{
private:
    Transform transform;

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
    CameraType type = CameraType::THIRD_PERSON;

public:
    Camera();
    void setPosition(glm::vec3 position);
    void setRotation(glm::vec3 rotation);
    void setPerspective(float fov, float aspectRatio, float near, float far);
    void setAspectRatio(float aspectRatio);

    glm::mat4 inline getView() const { return view; };
    glm::mat4 inline getProjection() const { return projection; };
    glm::vec3 inline getPosition() const { return transform.position; };
    glm::vec3 inline getRotation() const { return transform.rotation; };
};


class CameraControl {
protected:
    Camera *camera;

public:
    CameraControl(Camera *camera);
    virtual ~CameraControl();
    virtual void handleInput(GLFWwindow *window) = 0;
};


class OrbitControl : public CameraControl {
public:
    float orbitSensitivity = 0.2f;
    float zoomSensitivity = 0.0025f;

private:
    bool firstMouse;
    float lastMouseX;
    float lastMouseY;

public: 
    OrbitControl(Camera *camera);
    virtual void handleInput(GLFWwindow *window);
};
