#pragma once

#include <Systems/System.hpp>
#include <Utilities/Singleton.hpp>

class CameraControl final : public System, public Singleton<CameraControl>
{
public:
    virtual void Init() override;
    virtual void Update() override;

public:
    float m_orbitSensitivity = 0.005f;
    float m_zoomSensitivity  = 0.0025f;

private:
    double m_lastMouseX = 0.0f;
    double m_lastMouseY = 0.0f;
};
