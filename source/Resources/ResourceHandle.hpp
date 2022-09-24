#pragma once

#include <Systems/Renderer.hpp>
#include <Utilities/Helpers.hpp>

/// ResourceHandleBase
template<class ResourceType>
class ResourceHandleBase
{
public:
    virtual ResourceType& GetResource() = 0;
    virtual ResourceType const& GetResource() const = 0;
};

template<typename ResourceType>
using SharedResourceHandle = SharedPtr<ResourceHandleBase<ResourceType>>;

/// ResourceHandle
template<class ResourceType>
class ResourceHandle : public ResourceHandleBase<ResourceType>
{
public:
    ResourceHandle() = default;
    ResourceHandle(ResourceType& resource);
    ResourceHandle(ResourceType&& resource);
    
    virtual ResourceType& GetResource() override { return m_resource; }
    virtual ResourceType const& GetResource() const override { return m_resource; }

private:
    ResourceType m_resource;
};

template<typename ResourceType, class... CreateArgs>
inline ResourceHandle<ResourceType> CreateResourceHandle(CreateArgs... args)
{
    return ResourceHandle<ResourceType>(std::forward<CreateArgs>(args)...);
}

template<typename ResourceType>
inline SharedResourceHandle<ResourceType> MakeSharedResourceHandle(ResourceType& resource)
{
    return std::make_shared<ResourceHandle<ResourceType>>(resource);
}

template<typename ResourceType>
inline SharedResourceHandle<ResourceType> MakeSharedResourceHandle(ResourceType&& resource)
{
    return std::make_shared<ResourceHandle<ResourceType>>(std::move(resource));
}


/// ResourceHandleInFlight
template<class ResourceType>
class ResourceHandleInFlight : public ResourceHandleBase<ResourceType>
{
public:
    static auto constexpr FramesInFlight = Renderer::RenderSettings::m_maxFramesInFlight;

public:
    ResourceHandleInFlight() = default;
    ResourceHandleInFlight(std::array<ResourceType, ResourceHandleInFlight::FramesInFlight>& resources);
    ResourceHandleInFlight(std::array<ResourceType, ResourceHandleInFlight::FramesInFlight>&& resources);

    ResourceType& operator[](std::size_t idx) { return m_resourcesPerFrameInFlight[idx]; }
    ResourceType const& operator[](std::size_t idx) const { return m_resourcesPerFrameInFlight[idx]; }

    virtual ResourceType& GetResource() override;
    virtual ResourceType const& GetResource() const override;

    inline decltype(auto) begin() const noexcept { return m_resourcesPerFrameInFlight.begin(); }
    inline decltype(auto) begin() noexcept { return m_resourcesPerFrameInFlight.begin(); }
    inline decltype(auto) end() const noexcept { return m_resourcesPerFrameInFlight.end(); }
    inline decltype(auto) end() noexcept { return m_resourcesPerFrameInFlight.end(); }

private:
    std::array<ResourceType, ResourceHandleInFlight::FramesInFlight> m_resourcesPerFrameInFlight;
};

template<typename ResourceType>
inline SharedResourceHandle<ResourceType> MakeSharedResourceHandleInFlight(std::array<ResourceType, ResourceHandleInFlight<ResourceType>::FramesInFlight>& resources)
{
    return std::make_shared<ResourceHandleInFlight<ResourceType>>(resources);
}

template<typename ResourceType>
inline SharedResourceHandle<ResourceType> MakeSharedResourceHandleInFlight(std::array<ResourceType, ResourceHandleInFlight<ResourceType>::FramesInFlight>&& resources)
{
    return std::make_shared<ResourceHandleInFlight<ResourceType>>(std::move(resources));
}

template<typename ResourceType, class... CreateArgs>
inline ResourceHandleInFlight<ResourceType> CreateResourceHandleInFlight(CreateArgs... args)
{
    std::array<ResourceType, ResourceHandleInFlight<ResourceType>::FramesInFlight> resources;

    for (ResourceType& resource : resources)
    {
        resource = ResourceType(std::forward<CreateArgs>(args)...);
    }

    return ResourceHandleInFlight(std::move(resources));
}

template<typename ResourceType, typename CreateInfo>
inline ResourceHandleInFlight<ResourceType> CreateResourceHandleInFlight(std::array<CreateInfo, ResourceHandleInFlight<ResourceType>::FramesInFlight> const& infos)
{
    std::array<ResourceType, ResourceHandleInFlight<ResourceType>::FramesInFlight> resources;

    size_t infoIndex = 0;
    for (ResourceType& resource : resources)
    {
        resource = ResourceType(infos[infoIndex]);
        infoIndex++;
    }

    return ResourceHandleInFlight(std::move(resources));
}


/// ResourceHandle
template<class ResourceType>
ResourceHandle<ResourceType>::ResourceHandle(ResourceType& resource)
    : m_resource(resource)
{
}

template<class ResourceType>
ResourceHandle<ResourceType>::ResourceHandle(ResourceType&& resource)
    : m_resource(std::move(resource))
{
}


/// ResourceHandleInFlight
template<class ResourceType>
ResourceHandleInFlight<ResourceType>::ResourceHandleInFlight(std::array<ResourceType, ResourceHandleInFlight::FramesInFlight>& resources)
    : m_resourcesPerFrameInFlight(resources)
{
}

template<class ResourceType>
ResourceHandleInFlight<ResourceType>::ResourceHandleInFlight(std::array<ResourceType, ResourceHandleInFlight::FramesInFlight>&& resources)
    : m_resourcesPerFrameInFlight(std::move(resources))
{
}

template<class ResourceType>
ResourceType& ResourceHandleInFlight<ResourceType>::GetResource()
{
    Renderer const& renderer = Renderer::GetInstance();
    return m_resourcesPerFrameInFlight[renderer.GetCurrentFrame()];
}

template<class ResourceType>
ResourceType const& ResourceHandleInFlight<ResourceType>::GetResource() const
{
    Renderer const& renderer = Renderer::GetInstance();
    return m_resourcesPerFrameInFlight[renderer.GetCurrentFrame()];
}
