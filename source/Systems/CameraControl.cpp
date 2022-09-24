#include <Systems/CameraControl.hpp>

#include <Components/CameraComponent.hpp>
#include <Components/SceneComponent.hpp>
#include <Systems/EntitySystem.hpp>
#include <Systems/Renderer.hpp>

static void ClampAngle(float& angle)
{
    if (angle > glm::two_pi<float>())
        angle -= glm::two_pi<float>();
    else if (angle < -glm::two_pi<float>())
        angle += glm::two_pi<float>();
}

void CameraControl::Init()
{
    Renderer const& renderer = Renderer::GetInstance();
    renderer.GetMouseCursorPosition(m_lastMouseX, m_lastMouseY);
}

void CameraControl::Update()
{
    Renderer const& renderer = Renderer::GetInstance();
    EntitySystem& entitySystem = EntitySystem::GetInstance();

    double xPosition, yPosition;
    renderer.GetMouseCursorPosition(xPosition, yPosition);

    float const xOffset = m_lastMouseX - xPosition;
    float const yOffset = m_lastMouseY - yPosition;

    m_lastMouseX = xPosition;
    m_lastMouseY = yPosition;
    
    auto const& view = entitySystem.GetView<OrbitCameraControlComponent, SceneComponent>();
    Entity camera = view.front();
    if (!EntitySystem::IsEntityValid(camera))
    {
        return;
    }

    OrbitCameraControlComponent& controls = view.Get<OrbitCameraControlComponent>(camera);
    SceneComponent& scene = view.Get<SceneComponent>(camera);

    if (renderer.GetMouseButton(GLFW_MOUSE_BUTTON_LEFT))
    {
        controls.m_theta += xOffset * m_orbitSensitivity;
        controls.m_phi += yOffset * m_orbitSensitivity;

        ClampAngle(controls.m_theta);
        ClampAngle(controls.m_phi);
    }

    if (renderer.GetMouseButton(GLFW_MOUSE_BUTTON_RIGHT))
    {
        controls.m_radius += yOffset * m_zoomSensitivity;

        if (controls.m_radius < 0.0f)
            controls.m_radius = 0.0f;
    }

    glm::vec3 const position = glm::vec3(
        controls.m_radius * glm::sin(controls.m_phi) * glm::sin(controls.m_theta),
        controls.m_radius * glm::cos(controls.m_phi),
        controls.m_radius * glm::sin(controls.m_phi) * glm::cos(controls.m_theta)
    );

    glm::quat const rotation = glm::quat(glm::vec3(controls.m_phi - glm::half_pi<float>(), controls.m_theta, 0.0f));

    scene.SetWorldRotation(rotation);
    scene.SetWorldTranslation(position);
}
