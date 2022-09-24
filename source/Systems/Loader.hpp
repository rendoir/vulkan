#pragma once

#include <Systems/System.hpp>
#include <Utilities/Helpers.hpp>
#include <Utilities/Singleton.hpp>

class SceneComponent;
class TextureResource;
struct Material;
struct TextureSampler;


class Loader final : public System, public Singleton<Loader>
{
public:
    virtual void Init() override;
    virtual void Terminate() override;

    void LoadModel(entt::entity entity, std::string const& filePath, std::function<void(entt::entity)> const& nodeLoadedCallback = nullptr);
    SharedPtr<TextureResource> LoadTexture2D(std::string const& filePath);
    SharedPtr<TextureResource> LoadTextureCubeHdr(std::string const& folderName);

private:
    void OnFileDescriptorComponentCreated(entt::registry& registry, entt::entity entity);

    void LoadModelNode(SceneComponent& parentSceneComponent, tinygltf::Node const& gltfNode, tinygltf::Model const& gltfModel, std::vector<SharedPtr<Material>> const& materials, std::function<void(entt::entity)> const& nodeLoadedCallback = nullptr);
    void LoadMesh(entt::entity nodeEntity, tinygltf::Node const& gltfNode, tinygltf::Model const& gltfModel, std::vector<SharedPtr<Material>> const& materials);
    void LoadTextureSamplers(tinygltf::Model const& gltfModel, std::vector<TextureSampler>& textureSamplers);
    void LoadTextures(tinygltf::Model const& gltfModel, std::vector<SharedPtr<TextureResource>>& textures, std::vector<TextureSampler> const& textureSamplers);
    void LoadMaterials(tinygltf::Model& gltfModel, std::vector<SharedPtr<Material>>& materials, std::vector<SharedPtr<TextureResource>> const& textures);

    friend class Singleton<Loader>;
};