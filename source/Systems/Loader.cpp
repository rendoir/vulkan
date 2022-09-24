#include <Systems/Loader.hpp>

#include <Components/FileDescriptorComponent.hpp>
#include <Components/SceneComponent.hpp>
#include <Components/StaticMeshComponent.hpp>
#include <Resources/Descriptor.hpp>
#include <Resources/ImageResource.hpp>
#include <Resources/TextureResource.hpp>
#include <Systems/EntitySystem.hpp>

static bool IsValidTextureSampler(TextureSampler const& sampler);
static VkSamplerAddressMode GetVkWrapModeFromGltf(int32_t const wrapMode);
static VkFilter GetVkFilterModeFromGltf(int32_t const filterMode);
static VkFormat GetImageFormat(uint8_t channels, uint8_t bytesPerChannel, bool isHdr = false);

void Loader::Init()
{
    EntitySystem::GetInstance().AddOnConstructEvent<FileDescriptorComponent, &Loader::OnFileDescriptorComponentCreated>(*this);
}

void Loader::Terminate()
{
    EntitySystem::GetInstance().RemoveOnConstructEvent<FileDescriptorComponent, &Loader::OnFileDescriptorComponentCreated>(*this);
}

void Loader::OnFileDescriptorComponentCreated(entt::registry&, entt::entity entity)
{
    FileDescriptorComponent const& file = EntitySystem::GetInstance().GetComponent<FileDescriptorComponent>(entity);
    LoadModel(entity, file.GetModelFile(), file.GetNodeLoadedCallback());
}

void Loader::LoadModel(entt::entity entity, std::string const& filePath, std::function<void(entt::entity)> const& nodeLoadedCallback)
{
    Log("Load model file: %s", filePath.c_str());

    EntitySystem& entitySystem = EntitySystem::GetInstance();

    std::filesystem::path const extension = std::filesystem::path(filePath).extension();
    bool const binary = extension.string() == ".glb";

    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    std::string loadError, loadWarning;

    bool const fileLoaded = binary ? 
        gltfContext.LoadBinaryFromFile(&gltfModel, &loadError, &loadWarning, filePath.c_str()) :
        gltfContext.LoadASCIIFromFile(&gltfModel, &loadError, &loadWarning, filePath.c_str());

    if (!fileLoaded)
    {
        Warn("Failed to load model file: %s", filePath.c_str());
        return;
    }

    if (gltfModel.scenes.empty())
    {
        Warn("Model file has no scenes: %s", filePath.c_str());
        return;
    }

    std::vector<TextureSampler> textureSamplers;
    LoadTextureSamplers(gltfModel, textureSamplers);
    std::vector<SharedPtr<TextureResource>> textures;
    LoadTextures(gltfModel, textures, textureSamplers);
    std::vector<SharedPtr<Material>> materials;
    LoadMaterials(gltfModel, materials, textures);

    int32_t const sceneIndex = (gltfModel.defaultScene < 0) ? 0 : gltfModel.defaultScene;
    tinygltf::Scene const& gltfScene = gltfModel.scenes[sceneIndex];

    SceneComponent& rootSceneComponent = entitySystem.GetOrAddComponent<SceneComponent>(entity);    

    for (int32_t const& nodeIndex : gltfScene.nodes)
    {
        tinygltf::Node const& gltfNode = gltfModel.nodes[nodeIndex];
        LoadModelNode(rootSceneComponent, gltfNode, gltfModel, materials, nodeLoadedCallback);
    }
}

void Loader::LoadModelNode(SceneComponent& parentSceneComponent, tinygltf::Node const& gltfNode, tinygltf::Model const& gltfModel, std::vector<SharedPtr<Material>> const& materials, std::function<void(entt::entity)> const& nodeLoadedCallback)
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    Entity nodeEntity = entitySystem.CreateEntity();

    SceneComponent& nodeSceneComponent = entitySystem.AddComponent<SceneComponent>(nodeEntity);
    nodeSceneComponent.AttachTo(parentSceneComponent.GetEntity());

    // Local transform
    if (gltfNode.translation.size() == 3)
    {
        glm::vec3 const translation = glm::make_vec3(gltfNode.translation.data());
        nodeSceneComponent.SetLocalTranslation(translation);
    }
    
    if (gltfNode.rotation.size() == 4)
    {
        glm::quat const rotation = glm::make_quat(gltfNode.rotation.data());
        nodeSceneComponent.SetLocalRotation(rotation);
    }
    
    if (gltfNode.scale.size() == 3)
    {
        glm::vec3 const scale = glm::make_vec3(gltfNode.scale.data());
        nodeSceneComponent.SetLocalScale(scale);
    }

    // Node contains mesh data
    if (gltfNode.mesh >= 0)
    {
        LoadMesh(nodeEntity, gltfNode, gltfModel, materials);
    }

    // Load children
    for (int32_t const& childIndex : gltfNode.children)
    {
        tinygltf::Node const& childNode = gltfModel.nodes[childIndex];
        LoadModelNode(nodeSceneComponent, childNode, gltfModel, materials);
    }

    if (nodeLoadedCallback)
    {
        nodeLoadedCallback(nodeEntity);
    }
}

void Loader::LoadMesh(entt::entity nodeEntity, tinygltf::Node const& gltfNode, tinygltf::Model const& gltfModel, std::vector<SharedPtr<Material>> const& materials)
{
    EntitySystem& entitySystem = EntitySystem::GetInstance();
    StaticMeshComponent& nodeMeshComponent = entitySystem.AddComponent<StaticMeshComponent>(nodeEntity);

    tinygltf::Mesh const& gltfMesh = gltfModel.meshes[gltfNode.mesh];
    
    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;
    std::vector<Primitive> primitives;

    for (tinygltf::Primitive const& gltfPrimitive : gltfMesh.primitives)
    {
        uint64_t const vertexStart = vertices.size();
        uint32_t const indexStart = indices.size();
        uint64_t vertexCount = 0;
        uint32_t indexCount = 0;
        bool const hasIndices = gltfPrimitive.indices >= 0;
        
        // Vertices
        {
            float const* bufferPositions = nullptr;
            float const* bufferNormals = nullptr;
            float const* bufferTexCoordSet0 = nullptr;
            float const* bufferTexCoordSet1 = nullptr;

            // Position attribute is required
            auto const& positionAttributeIt = gltfPrimitive.attributes.find("POSITION");
            if (positionAttributeIt == gltfPrimitive.attributes.end())
            {
                Warn("Model primitive has no position attribute.");
                continue;
            }

            tinygltf::Accessor const& positionAccessor = gltfModel.accessors[positionAttributeIt->second];
            tinygltf::BufferView const& positionView = gltfModel.bufferViews[positionAccessor.bufferView];
            bufferPositions = reinterpret_cast<float const*>(&(gltfModel.buffers[positionView.buffer].data[positionAccessor.byteOffset + positionView.byteOffset]));
            vertexCount = static_cast<uint32_t>(positionAccessor.count);

            auto const& normalAttributeIt = gltfPrimitive.attributes.find("NORMAL");
            if (normalAttributeIt != gltfPrimitive.attributes.end())
            {
                tinygltf::Accessor const& normalmAccessor = gltfModel.accessors[normalAttributeIt->second];
                tinygltf::BufferView const& normalView = gltfModel.bufferViews[normalmAccessor.bufferView];
                bufferNormals = reinterpret_cast<float const*>(&(gltfModel.buffers[normalView.buffer].data[normalmAccessor.byteOffset + normalView.byteOffset]));
            }

            auto const& uvSet0AttributeIt = gltfPrimitive.attributes.find("TEXCOORD_0");
            if (uvSet0AttributeIt != gltfPrimitive.attributes.end())
            {
                tinygltf::Accessor const& uvAccessor = gltfModel.accessors[uvSet0AttributeIt->second];
                tinygltf::BufferView const& uvView = gltfModel.bufferViews[uvAccessor.bufferView];
                bufferTexCoordSet0 = reinterpret_cast<float const*>(&(gltfModel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
            }

            auto const& uvSet1AttributeIt = gltfPrimitive.attributes.find("TEXCOORD_1");
            if (uvSet1AttributeIt != gltfPrimitive.attributes.end())
            {
                tinygltf::Accessor const& uvAccessor = gltfModel.accessors[uvSet1AttributeIt->second];
                tinygltf::BufferView const& uvView = gltfModel.bufferViews[uvAccessor.bufferView];
                bufferTexCoordSet1 = reinterpret_cast<float const*>(&(gltfModel.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
            }

            for (uint64_t v = 0; v < positionAccessor.count; v++)
            {
                Vertex vertex = {};
                vertex.m_position = glm::make_vec3(&bufferPositions[v * 3]);
                vertex.m_normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
                vertex.m_uvSet0 = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * 2]) : glm::vec3(0.0f);
                vertex.m_uvSet1 = bufferTexCoordSet1 ? glm::make_vec2(&bufferTexCoordSet1[v * 2]) : glm::vec3(0.0f);

                vertices.push_back(vertex);
            }
        }

        // Indices
        if (hasIndices)
        {
            tinygltf::Accessor const& accessor = gltfModel.accessors[gltfPrimitive.indices];
            tinygltf::BufferView const& bufferView = gltfModel.bufferViews[accessor.bufferView];
            tinygltf::Buffer const& buffer = gltfModel.buffers[bufferView.buffer];

            indexCount = accessor.count;
            void const* const rawDataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

            switch (accessor.componentType)
            {
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: 
            {
                uint32_t const* dataPtr = static_cast<uint32_t const*>(rawDataPtr);
                for (size_t index = 0; index < accessor.count; index++)
                {
                    indices.push_back(dataPtr[index] + vertexStart);
                }
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: 
            {
                uint16_t const* dataPtr = static_cast<uint16_t const*>(rawDataPtr);
                for (size_t index = 0; index < accessor.count; index++)
                {
                    indices.push_back(dataPtr[index] + vertexStart);
                }
                break;
            }
            case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: 
            {
                uint8_t const* dataPtr = static_cast<uint8_t const*>(rawDataPtr);
                for (size_t index = 0; index < accessor.count; index++)
                {
                    indices.push_back(dataPtr[index] + vertexStart);
                }
                break;
            }
            default:
            {
                ThrowError("[Loader] Index type %d not supported", accessor.componentType);
                continue;
            }
            }
        }

        Primitive primitive = {};
        primitive.m_firstIndex = indexStart;
        primitive.m_indexCount = indexCount;
        primitive.m_vertexCount = vertexCount;
        primitive.m_hasIndices = hasIndices;
        primitive.m_material = gltfPrimitive.material >= 0 ? materials[gltfPrimitive.material] : nullptr;
        
        primitives.push_back(primitive);
    }

    nodeMeshComponent.SetVertices(vertices);
    nodeMeshComponent.SetIndices(indices);
    nodeMeshComponent.SetPrimitives(primitives);
}

void Loader::LoadTextureSamplers(tinygltf::Model const& gltfModel, std::vector<struct TextureSampler>& textureSamplers)
{
    for (tinygltf::Sampler const& gltfSampler : gltfModel.samplers)
    {
        TextureSampler sampler = {};
        sampler.m_minFilter = GetVkFilterModeFromGltf(gltfSampler.minFilter);
        sampler.m_magFilter = GetVkFilterModeFromGltf(gltfSampler.magFilter);
        sampler.m_addressModeU = GetVkWrapModeFromGltf(gltfSampler.wrapS);
        sampler.m_addressModeV = GetVkWrapModeFromGltf(gltfSampler.wrapT);
        sampler.m_addressModeW = sampler.m_addressModeV;
        textureSamplers.push_back(sampler);
    }
}

void Loader::LoadTextures(tinygltf::Model const& gltfModel, std::vector<SharedPtr<TextureResource>>& textures, std::vector<struct TextureSampler> const& textureSamplers)
{
    for (tinygltf::Texture const& gltfTexture : gltfModel.textures)
    {
        tinygltf::Image const& gltfImage = gltfModel.images[gltfTexture.source];
        
        TextureSampler textureSampler = {};
        if (gltfTexture.sampler >= 0 && IsValidTextureSampler(textureSamplers[gltfTexture.sampler]))
        {
            textureSampler = textureSamplers[gltfTexture.sampler];
        }

        TextureCreationInfo textureInfo;
        textureInfo.m_imageCreateInfo.m_width = static_cast<float>(gltfImage.width);
        textureInfo.m_imageCreateInfo.m_height = static_cast<float>(gltfImage.height);
        textureInfo.m_imageCreateInfo.m_sizeType = SizeType::Absolute;
        textureInfo.m_imageCreateInfo.m_layers = 1;
        textureInfo.m_imageCreateInfo.m_mipLevels = UINT8_MAX;
        textureInfo.m_channels = gltfImage.component;
        textureInfo.m_bytesPerChannel = gltfImage.bits / 8;
        textureInfo.m_imageCreateInfo.m_format = GetImageFormat(textureInfo.m_channels, textureInfo.m_bytesPerChannel);
        textureInfo.m_samplerInfo = textureSampler;
        textureInfo.m_data = gltfImage.image.data();

        SharedPtr<TextureResource> texture = std::make_shared<TextureResource>(textureInfo);
        textures.emplace_back(texture);
    }
}

void Loader::LoadMaterials(tinygltf::Model& gltfModel, std::vector<SharedPtr<Material>>& materials, std::vector<SharedPtr<TextureResource>> const& textures)
{
    for (tinygltf::Material& gltfMaterial : gltfModel.materials)
    {
        SharedPtr<Material> material = std::make_shared<Material>();

        if (gltfMaterial.values.find("roughnessFactor") != gltfMaterial.values.end())
        {
            material->m_roughnessFactor = static_cast<float>(gltfMaterial.values["roughnessFactor"].Factor());
        }
        if (gltfMaterial.values.find("metallicFactor") != gltfMaterial.values.end())
        {
            material->m_metallicFactor = static_cast<float>(gltfMaterial.values["metallicFactor"].Factor());
        }
        if (gltfMaterial.values.find("baseColorFactor") != gltfMaterial.values.end())
        {
            material->m_baseColorFactor = glm::make_vec4(gltfMaterial.values["baseColorFactor"].ColorFactor().data());
        }
        if (gltfMaterial.additionalValues.find("emissiveFactor") != gltfMaterial.additionalValues.end())
        {
            material->m_emissiveFactor = glm::make_vec3(gltfMaterial.additionalValues["emissiveFactor"].ColorFactor().data());
        }
        if (gltfMaterial.values.find("baseColorTexture") != gltfMaterial.values.end())
        {
            material->m_baseColorTexture = textures[gltfMaterial.values["baseColorTexture"].TextureIndex()];
            material->m_baseColorTextCoordSet = gltfMaterial.values["baseColorTexture"].TextureTexCoord();
        }
        if (gltfMaterial.values.find("metallicRoughnessTexture") != gltfMaterial.values.end())
        {
            material->m_metallicRoughnessTexture = textures[gltfMaterial.values["metallicRoughnessTexture"].TextureIndex()];
            material->m_metallicRoughnessTextCoordSet = gltfMaterial.values["metallicRoughnessTexture"].TextureTexCoord();
        }
        if (gltfMaterial.additionalValues.find("normalTexture") != gltfMaterial.additionalValues.end())
        {
            material->m_normalTexture = textures[gltfMaterial.additionalValues["normalTexture"].TextureIndex()];
            material->m_normalTextCoordSet = gltfMaterial.additionalValues["normalTexture"].TextureTexCoord();
        }
        if (gltfMaterial.additionalValues.find("emissiveTexture") != gltfMaterial.additionalValues.end())
        {
            material->m_emissiveTexture = textures[gltfMaterial.additionalValues["emissiveTexture"].TextureIndex()];
            material->m_emissiveTextCoordSet = gltfMaterial.additionalValues["emissiveTexture"].TextureTexCoord();
        }
        if (gltfMaterial.additionalValues.find("occlusionTexture") != gltfMaterial.additionalValues.end())
        {
            material->m_occlusionTexture = textures[gltfMaterial.additionalValues["occlusionTexture"].TextureIndex()];
            material->m_occlusionTextCoordSet = gltfMaterial.additionalValues["occlusionTexture"].TextureTexCoord();
        }
        if (gltfMaterial.additionalValues.find("alphaMode") != gltfMaterial.additionalValues.end())
        {
            tinygltf::Parameter param = gltfMaterial.additionalValues["alphaMode"];
            if (param.string_value == "BLEND")
            {
                material->m_alphaMode = Material::AlphaMode::Blend;
            }
            else if (param.string_value == "MASK")
            {
                material->m_alphaMode = Material::AlphaMode::Mask;
            }
        }
        if (gltfMaterial.additionalValues.find("alphaCutoff") != gltfMaterial.additionalValues.end())
        {
            material->m_alphaCutoff = static_cast<float>(gltfMaterial.additionalValues["alphaCutoff"].Factor());
        }

        material->Init();

        materials.push_back(material);
    }
}

SharedPtr<TextureResource> Loader::LoadTexture2D(std::string const& filePath)
{
    Log("Load texture 2D: %s", filePath.c_str());

    TextureCreationInfo textureInfo;

    int32_t width = 0;
    int32_t height = 0;

    uint8_t* imageData = stbi_load(filePath.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
    if (!imageData)
    {
        ThrowError("Failed to load texture: %s.", filePath.c_str());
    }

    textureInfo.m_imageCreateInfo.m_width = static_cast<float>(width);
    textureInfo.m_imageCreateInfo.m_height = static_cast<float>(height);
    textureInfo.m_imageCreateInfo.m_sizeType = SizeType::Absolute;
    textureInfo.m_imageCreateInfo.m_layers = 1;
    textureInfo.m_imageCreateInfo.m_mipLevels = UINT8_MAX;
    textureInfo.m_imageCreateInfo.m_format = GetImageFormat(textureInfo.m_channels, textureInfo.m_bytesPerChannel);
    textureInfo.m_channels = STBI_rgb_alpha;
    textureInfo.m_bytesPerChannel = sizeof(*imageData);
    textureInfo.m_data = imageData;

    SharedPtr<TextureResource> texture = std::make_shared<TextureResource>(textureInfo);

    stbi_image_free(imageData);

    return texture;
}

SharedPtr<TextureResource> Loader::LoadTextureCubeHdr(std::string const& folderName)
{
    Log("Load texture cube hdr: %s", folderName.c_str());

    static std::string const fileNames[] = { "negx.hdr", "posx.hdr", "negy.hdr", "posy.hdr", "negz.hdr", "posz.hdr" };
    
    TextureCreationInfo textureInfo;

    int32_t width = 0;
    int32_t height = 0;

    float* cubeData[6];
    for(uint8_t i = 0; i < 6; i++)
    {
        std::string const filePath = folderName + '/' + fileNames[i];
        cubeData[i] = stbi_loadf(filePath.c_str(), &width, &height, nullptr, STBI_rgb_alpha);

        if(!cubeData[i])
        {
            ThrowError("Failed to load texture: %s.", filePath.c_str());
        }
    }

    textureInfo.m_imageCreateInfo.m_width = static_cast<float>(width);
    textureInfo.m_imageCreateInfo.m_height = static_cast<float>(height);
    textureInfo.m_imageCreateInfo.m_sizeType = SizeType::Absolute;
    textureInfo.m_channels = STBI_rgb_alpha;
    textureInfo.m_bytesPerChannel = sizeof(**cubeData);
    textureInfo.m_imageCreateInfo.m_layers = 6;
    textureInfo.m_imageCreateInfo.m_mipLevels = UINT8_MAX;
    textureInfo.m_imageCreateInfo.m_format = GetImageFormat(textureInfo.m_channels, textureInfo.m_bytesPerChannel, true);

    VkExtent2D const extent = ImageResource::GetExtent(textureInfo.m_imageCreateInfo);
    uint64_t const sizePerFace = extent.width * extent.height * textureInfo.m_channels;
    textureInfo.m_data = new float[sizePerFace * 6];

    uint64_t memoryOffset = 0;
    for(uint8_t i = 0; i < 6; i++)
    {
        float* destination = static_cast<float*>(const_cast<void*>(textureInfo.m_data)) + memoryOffset; 
        memcpy(destination, cubeData[i], sizePerFace * textureInfo.m_bytesPerChannel);
        memoryOffset += sizePerFace;

        stbi_image_free(cubeData[i]);
    }

    SharedPtr<TextureResource> texture = std::make_shared<TextureResource>(textureInfo);

    delete textureInfo.m_data;

    return texture;
}

static bool IsValidTextureSampler(TextureSampler const& sampler)
{
    return sampler.m_minFilter != VK_FILTER_MAX_ENUM
        && sampler.m_magFilter != VK_FILTER_MAX_ENUM
        && sampler.m_addressModeU != VK_SAMPLER_ADDRESS_MODE_MAX_ENUM
        && sampler.m_addressModeV != VK_SAMPLER_ADDRESS_MODE_MAX_ENUM
        && sampler.m_addressModeW != VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
}

static VkSamplerAddressMode GetVkWrapModeFromGltf(int32_t const wrapMode) 
{
    switch (wrapMode)
    {
    case TINYGLTF_TEXTURE_WRAP_REPEAT:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case TINYGLTF_TEXTURE_WRAP_CLAMP_TO_EDGE:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case TINYGLTF_TEXTURE_WRAP_MIRRORED_REPEAT:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    default:
        return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
    }
}

static VkFilter GetVkFilterModeFromGltf(int32_t const filterMode) 
{
    switch (filterMode)
    {
    case TINYGLTF_TEXTURE_FILTER_NEAREST:
        return VK_FILTER_NEAREST;
    case TINYGLTF_TEXTURE_FILTER_LINEAR:
        return VK_FILTER_LINEAR;
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_NEAREST:
        return VK_FILTER_NEAREST;
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_NEAREST:
        return VK_FILTER_NEAREST;
    case TINYGLTF_TEXTURE_FILTER_NEAREST_MIPMAP_LINEAR:
        return VK_FILTER_LINEAR;
    case TINYGLTF_TEXTURE_FILTER_LINEAR_MIPMAP_LINEAR:
        return VK_FILTER_LINEAR;
    default:
        return VK_FILTER_MAX_ENUM;
    }
}

static VkFormat GetImageFormat(uint8_t channels, uint8_t bytesPerChannel, bool isHdr)
{
    if (!isHdr)
    {
        if (channels == 1 && bytesPerChannel == 1) return VK_FORMAT_R8_UNORM;
        if (channels == 2 && bytesPerChannel == 1) return VK_FORMAT_R8G8_UNORM;
        if (channels == 3 && bytesPerChannel == 1) return VK_FORMAT_R8G8B8_UNORM;
        if (channels == 4 && bytesPerChannel == 1) return VK_FORMAT_R8G8B8A8_UNORM;

        if (channels == 1 && bytesPerChannel == 2) return VK_FORMAT_R16_UNORM;
        if (channels == 2 && bytesPerChannel == 2) return VK_FORMAT_R16G16_UNORM;
        if (channels == 3 && bytesPerChannel == 2) return VK_FORMAT_R16G16B16_UNORM;
        if (channels == 4 && bytesPerChannel == 2) return VK_FORMAT_R16G16B16A16_UNORM;
    }
    else
    {
        if (channels == 1 && bytesPerChannel == 4) return VK_FORMAT_R32_SFLOAT;
        if (channels == 2 && bytesPerChannel == 4) return VK_FORMAT_R32G32_SFLOAT;
        if (channels == 3 && bytesPerChannel == 4) return VK_FORMAT_R32G32B32_SFLOAT;
        if (channels == 4 && bytesPerChannel == 4) return VK_FORMAT_R32G32B32A32_SFLOAT;

        if (channels == 1 && bytesPerChannel == 8) return VK_FORMAT_R64_SFLOAT;
        if (channels == 2 && bytesPerChannel == 8) return VK_FORMAT_R64G64_SFLOAT;
        if (channels == 3 && bytesPerChannel == 8) return VK_FORMAT_R64G64B64_SFLOAT;
        if (channels == 4 && bytesPerChannel == 8) return VK_FORMAT_R64G64B64A64_SFLOAT;
    }

    return VK_FORMAT_UNDEFINED;
}
