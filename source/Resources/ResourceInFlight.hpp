#pragma once

#include <Systems/Renderer.hpp>
#include <Utilities/Helpers.hpp>

template<class ResourceType>
class ResourceInFlight
{
public:
    static auto constexpr FramesInFlight = Renderer::RenderSettings::m_maxFramesInFlight;

public:
    ResourceInFlight() = default;
    explicit ResourceInFlight(std::array<ResourceType, ResourceInFlight::FramesInFlight>&& resources)
        : m_resourcesPerFrameInFlight(std::move(resources))
    {
    }

    ResourceInFlight(const ResourceInFlight&) = delete;
    ResourceInFlight& operator=(const ResourceInFlight&) = delete;

    ResourceInFlight(ResourceInFlight&&) noexcept = default;
    ResourceInFlight& operator=(ResourceInFlight&&) noexcept = default;

    ResourceType& operator[](std::size_t idx) { return const_cast<ResourceType&>(std::as_const(*this)[idx]); }

    ResourceType const& operator[](std::size_t idx) const
    {
        assert(idx < FramesInFlight);
        return m_resourcesPerFrameInFlight[idx];
    }

    ResourceType& GetResource() { return const_cast<ResourceType&>(std::as_const(*this).GetResource()); }
    ResourceType const& GetResource() const
    {
        Renderer const& renderer = Renderer::GetInstance();
        const std::size_t idx = renderer.GetCurrentFrame();
        assert(idx < FramesInFlight);
        return m_resourcesPerFrameInFlight[idx];
    }

    decltype(auto) begin() const noexcept { return m_resourcesPerFrameInFlight.begin(); }
    decltype(auto) begin() noexcept { return m_resourcesPerFrameInFlight.begin(); }
    decltype(auto) end() const noexcept { return m_resourcesPerFrameInFlight.end(); }
    decltype(auto) end() noexcept { return m_resourcesPerFrameInFlight.end(); }

private:
    std::array<ResourceType, ResourceInFlight::FramesInFlight> m_resourcesPerFrameInFlight;
};

template<typename ResourceType, class... CreateArgs>
ResourceInFlight<ResourceType> CreateResourceInFlight(CreateArgs const&... args)
{
    std::array<ResourceType, ResourceInFlight<ResourceType>::FramesInFlight> resources;

    for (std::size_t i = 0; i < resources.size(); ++i)
    {
        resources[i] = ResourceType(args...);
    }

    return ResourceInFlight<ResourceType>(std::move(resources));
}

template<typename ResourceType, typename CreateInfo>
ResourceInFlight<ResourceType> CreateResourceInFlight(std::array<CreateInfo, ResourceInFlight<ResourceType>::FramesInFlight> const& infos)
{
    std::array<ResourceType, ResourceInFlight<ResourceType>::FramesInFlight> resources;

    for (std::size_t i = 0; i < infos.size(); ++i)
    {
        resources[i] = ResourceType(infos[i]);
    }

    return ResourceInFlight<ResourceType>(std::move(resources));
}
