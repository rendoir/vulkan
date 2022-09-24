#include <Components/CameraComponent.hpp>
#include <Components/FileDescriptorComponent.hpp>
#include <Components/LightComponent.hpp>
#include <Components/SceneComponent.hpp>
#include <Components/SkyboxComponent.hpp>
#include <Components/StaticMeshComponent.hpp>
#include <Engine.hpp>
#include <Resources/TextureResource.hpp>
#include <Systems/EntitySystem.hpp>
#include <Systems/CameraControl.hpp>
#include <Systems/Loader.hpp>
#include <Systems/RenderPasses/ShadingPass.hpp>
#include <Systems/RenderPasses/SkyboxPass.hpp>
#include <Systems/Renderer.hpp>
#include <Systems/RenderGraph.hpp>
#include <Systems/System.hpp>
#include <Utilities/Helpers.hpp>
#include <Utilities/Singleton.hpp>

class TestSystem : public System, public Singleton<TestSystem>
{
public:
    virtual void Init() override;
    virtual void Update() override;
};

int main(int, char*[])
{
    Engine& engine = Engine::GetInstance();
    engine.AddApplicationSystem<TestSystem>();
    engine.AddApplicationSystem<CameraControl>();

    engine.Run();

    return 0;
}

void TestSystem::Init()
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();

    Entity helmet = entitySystem.CreateEntity();
    entitySystem.AddComponent<FileDescriptorComponent>(helmet, "resources/models/DamagedHelmet/DamagedHelmet.gltf");
    
    //Entity spheres = entitySystem.CreateEntity();
    //entitySystem.AddComponent<FileDescriptorComponent>(spheres, "resources/models/Spheres/spheres.glb");

    Entity camera = entitySystem.CreateEntity();
    entitySystem.AddComponent<SceneComponent>(camera);
    entitySystem.AddComponent<CameraComponent>(camera);
    OrbitCameraControlComponent& orbitComponent = entitySystem.AddComponent<OrbitCameraControlComponent>(camera);
    orbitComponent.m_radius = 5.0f;
    CameraControl::GetInstance().Update();

    SharedPtr<TextureResource> hdrCubeTexture = Loader::GetInstance().LoadTextureCubeHdr("resources/textures/papermill");

    Entity skybox = entitySystem.CreateEntity();
    entitySystem.AddComponent<FileDescriptorComponent>(skybox, "resources/models/Skybox/Skybox.glb", [&hdrCubeTexture](entt::entity node) { EntitySystem::GetInstance().AddComponent<SkyboxComponent>(node, hdrCubeTexture); });
    
    Entity lightEntity0 = entitySystem.CreateEntity();
    LightComponent& light0 = entitySystem.AddComponent<LightComponent>(lightEntity0);
    light0.m_color = glm::vec3(4.0f, 0.0f, 0.0f);
    SceneComponent& lightScene0 = entitySystem.AddComponent<SceneComponent>(lightEntity0);
    lightScene0.SetWorldTranslation(glm::vec3(1.0f, 1.0f, 1.0f));

    Entity lightEntity1 = entitySystem.CreateEntity();
    LightComponent& light1 = entitySystem.AddComponent<LightComponent>(lightEntity1);
    light1.m_color = glm::vec3(0.0f, 0.0f, 4.0f);
    SceneComponent& lightScene1 = entitySystem.AddComponent<SceneComponent>(lightEntity1);
    lightScene1.SetWorldTranslation(glm::vec3(-1.0f, 1.0f, 1.0f));

    //SharedPtr<TextureResource> uvGridTexture = Loader::GetInstance().LoadTexture2D("resources/textures/uv_grid.jpg");
}

void TestSystem::Update()
{
    //EntitySystem& entitySystem = EntitySystem::GetInstance();
    //Renderer& renderer = Renderer::GetInstance();

    /*
    auto const& view = entitySystem.GetView<SceneComponent>(entt::exclude_t<CameraComponent, LightComponent>());
    for (entt::entity e : view)
    {
        if (renderer.GetKey(GLFW_KEY_W) == GLFW_PRESS)
        {
            SceneComponent& scene = view.Get<SceneComponent>(e);
            if (EntitySystem::IsEntityValid(scene.GetParent()))
            {
                continue;
            }

            glm::quat deltaQuat = glm::quat(glm::vec3(0.0f, 0.001f, 0.0f));
            scene.SetWorldRotation(scene.GetWorldRotation() * deltaQuat);
            scene.SetWorldTranslation(scene.GetWorldTranslation() + glm::vec3(0.001f, 0.0f, 0.0f));
        }
    }
    */

    /*
    if (renderer.GetKey(GLFW_KEY_Z) == GLFW_PRESS)
    {
        renderer.GetRenderGraph()->AddPass<ShadingPass>("shading");
    }

    if (renderer.GetKey(GLFW_KEY_X) == GLFW_PRESS)
    {
        renderer.GetRenderGraph()->RemovePass("shading");
    }
    */

    /*
    if (renderer.GetKey(GLFW_KEY_L) == GLFW_PRESS)
    {
        Entity lightEntity = entitySystem.CreateEntity();
        LightComponent& light = entitySystem.AddComponent<LightComponent>(lightEntity);
        light.m_color = glm::vec3(0.0f, 1.0f, 0.0f);
        SceneComponent& lightScene = entitySystem.AddComponent<SceneComponent>(lightEntity);
        lightScene.SetWorldTranslation(glm::vec3(0.0f, 1.0f, 1.0f));
    }
    */
}
