#pragma once

#include <Utilities/Singleton.hpp>

class System;

class Engine final : public Singleton<Engine>
{    
public:
    void Run();

    template<typename SystemType>
    inline void AddEngineSystem() { m_engineSystems.push_back(&SystemType::GetInstance()); }

    template<typename SystemType>
    inline void AddApplicationSystem() { m_applicationSystems.push_back(&SystemType::GetInstance()); }
    
private:
    Engine();
    virtual ~Engine() = default;

    void Init();
    void PostInit();
    void PreTerminate();
    void Terminate();
    void Update();
    bool ShouldExit();

private:
    std::vector<System*> m_engineSystems;
    std::vector<System*> m_applicationSystems;

    friend class Singleton<Engine>;
};