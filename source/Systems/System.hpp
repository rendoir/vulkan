#pragma once

class System
{
public:
    virtual void Init() { }
    virtual void PostInit() { }
    virtual void PreTerminate() { }
    virtual void Terminate() { }
    
    virtual void Update() { }
};
