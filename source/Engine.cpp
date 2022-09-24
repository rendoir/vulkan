#include <Engine.hpp>

#include <Components/CameraComponent.hpp>
#include <Components/LightComponent.hpp>
#include <Components/SceneComponent.hpp>
#include <Components/SkyboxComponent.hpp>
#include <Systems/DirtySystem.hpp>
#include <Systems/EntitySystem.hpp>
#include <Systems/Loader.hpp>
#include <Systems/Renderer.hpp>
#include <Systems/ResourceManager.hpp>
#include <Systems/System.hpp>
#include <Utilities/Helpers.hpp>

Engine::Engine()
{
    AddEngineSystem<Renderer>();
    AddEngineSystem<ResourceManager>();
    AddEngineSystem<Loader>();
    AddEngineSystem<EntitySystem>();
    AddEngineSystem<SceneResourceSystem>();
    AddEngineSystem<CameraResourceSystem>();
    AddEngineSystem<SkyboxResourceSystem>();
    AddEngineSystem<LightGlobalResourceSystem>();
    AddEngineSystem<DirtySystem>();
}

void Engine::Init()
{
    for (System* system : m_engineSystems)
    {
        system->Init();
    }

    for (System* system : m_applicationSystems)
    {
        system->Init();
    }
}

void Engine::PostInit()
{
    for (System* system : m_engineSystems)
    {
        system->PostInit();
    }

    for (System* system : m_applicationSystems)
    {
        system->PostInit();
    }
}

void Engine::PreTerminate()
{
    for (System* system : Reverse(m_applicationSystems))
    {
        system->PreTerminate();
    }

    for (System* system : Reverse(m_engineSystems))
    {
        system->PreTerminate();
    }
}

void Engine::Terminate()
{
    for (System* system : Reverse(m_applicationSystems))
    {
        system->Terminate();
    }

    for (System* system : Reverse(m_engineSystems))
    {
        system->Terminate();
    }
}

void Engine::Update()
{
    for (System* system : m_engineSystems)
    {
        system->Update();
    }

    for (System* system : m_applicationSystems)
    {
        system->Update();
    }
}

void Engine::Run()
{
    Init();
    PostInit();

    while (!ShouldExit())
    {
        Update();
    }

    PreTerminate();
    Terminate();
}

bool Engine::ShouldExit()
{
    return Renderer::GetInstance().ShouldExit();
}
