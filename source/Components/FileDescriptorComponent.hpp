#pragma once

#include <Components/EntityComponent.hpp>

class FileDescriptorComponent : public EntityComponent
{
public:
    FileDescriptorComponent(std::string const& modelFile, std::function<void(entt::entity)> const& nodeLoadedCallback = nullptr)
        : m_modelFile(modelFile)
        , m_nodeLoadedCallback(nodeLoadedCallback)
    {
    }
    virtual ~FileDescriptorComponent() = default;

    std::string const& GetModelFile() const { return m_modelFile; }
    std::function<void(entt::entity)> const& GetNodeLoadedCallback() const { return m_nodeLoadedCallback; }

private:
    std::string m_modelFile;
    std::function<void(entt::entity)> m_nodeLoadedCallback = nullptr;
};