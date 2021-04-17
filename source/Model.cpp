#include "Model.hpp"
#include "Renderer.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_IMPLEMENTATION
#include <glTF/tiny_gltf.h>

#include <iostream>


BoundingBox BoundingBox::FromMatrix(glm::mat4 const& matrix) {
    glm::vec3 min = glm::vec3(matrix[3]);
    glm::vec3 max = min;
    glm::vec3 v0, v1;
    
    glm::vec3 right = glm::vec3(matrix[0]);
    v0 = right * this->m_min.x;
    v1 = right * this->m_max.x;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    glm::vec3 up = glm::vec3(matrix[1]);
    v0 = up * this->m_min.y;
    v1 = up * this->m_max.y;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    glm::vec3 back = glm::vec3(matrix[2]);
    v0 = back * this->m_min.z;
    v1 = back * this->m_max.z;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    return BoundingBox(min, max);
}


void Texture2D::FromglTfImage(tinygltf::Image const& gltfimage, TextureSampler const& textureSampler, Renderer* renderer) {
    this->m_renderer = renderer;

    uint8_t const* const imageData = gltfimage.image.data();
    VkDeviceSize imageSize = gltfimage.image.size();
    m_width = gltfimage.width;
    m_height = gltfimage.height;
    m_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO - Make parameter

    CreateTextureImage(imageData, imageSize);
    CreateTextureImageView();
    CreateTextureSampler(textureSampler);
    
    m_descriptor.sampler = m_sampler;
    m_descriptor.imageView = m_view;
    m_descriptor.imageLayout = m_imageLayout;
}

void Texture2D::FromFile(std::string const filename, Renderer* renderer) {
    this->m_renderer = renderer;

    int channels;
    stbi_uc* imageData = stbi_load(filename.c_str(), (int*)&m_width, (int*)&m_height, &channels, STBI_rgb_alpha);
    VkDeviceSize imageSize = m_width * m_height * STBI_rgb_alpha;
    m_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO - Make parameter

    if (!imageData) {
        throw std::runtime_error("failed to load texture image!");
    }

    CreateTextureImage(imageData, imageSize);
    CreateTextureImageView();
    CreateTextureSampler(TextureSampler());

    m_descriptor.sampler = m_sampler;
    m_descriptor.imageView = m_view;
    m_descriptor.imageLayout = m_imageLayout;

    stbi_image_free(imageData);
}

void Texture2D::CreateTextureImage(uint8_t const* const imageData, VkDeviceSize const& imageSize) {
    m_mipLevels = static_cast<uint32_t>(floor(log2(std::max(m_width, m_height))) + 1);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    m_renderer->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(m_renderer->m_device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, imageData, static_cast<size_t>(imageSize));
    vkUnmapMemory(m_renderer->m_device, stagingBufferMemory);

    m_renderer->CreateImage(m_width, m_height, m_mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_deviceMemory);

    m_renderer->TransitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels);
    m_renderer->CopyBufferToImage(stagingBuffer, m_image, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
    //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

    vkDestroyBuffer(m_renderer->m_device, stagingBuffer, nullptr);
    vkFreeMemory(m_renderer->m_device, stagingBufferMemory, nullptr);

    m_renderer->GenerateMipmaps(m_image, VK_FORMAT_R8G8B8A8_UNORM, m_width, m_height, m_mipLevels);
}

void Texture2D::CreateTextureImageView() {
    m_view = m_renderer->CreateImageView(m_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels);
}

void Texture2D::CreateTextureSampler(TextureSampler const& textureSampler) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = textureSampler.m_magFilter;
    samplerInfo.minFilter = textureSampler.m_minFilter;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = textureSampler.m_addressModeU;
    samplerInfo.addressModeV = textureSampler.m_addressModeV;
    samplerInfo.addressModeW = textureSampler.m_addressModeW;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = static_cast<float>(m_mipLevels);
    samplerInfo.mipLodBias = 0;

    if (vkCreateSampler(m_renderer->m_device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }   
}

void Texture2D::Destroy()
{
    vkDestroyImageView(m_renderer->m_device, m_view, nullptr);
    vkDestroyImage(m_renderer->m_device, m_image, nullptr);
    vkFreeMemory(m_renderer->m_device, m_deviceMemory, nullptr);
    vkDestroySampler(m_renderer->m_device, m_sampler, nullptr);
}


void Texture3D::FromFolder(std::string const folderName, Renderer* renderer) {
    this->m_renderer = renderer;
    std::string fileNames[] = { "negx.hdr", "posx.hdr", "negy.hdr", "posy.hdr", "negz.hdr", "posz.hdr" }; // TODO - Should be a parameter?
    
    float *imageData[6];
    for(int i = 0; i < 6; i++) {
        int channels;
        imageData[i] = stbi_loadf((folderName + '/' + fileNames[i]).c_str(), (int*)&m_width, (int*)&m_height, &channels, STBI_rgb_alpha);

        if(!imageData[i]) {
            std::cerr << "Error loading image: " << folderName  << std::endl;
            assert(imageData[i] != nullptr);
        }
    }

    m_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO - Make parameter
    m_faceSize = m_width * m_height * STBI_rgb_alpha * 4;

    CreateTextureImage(imageData);
    CreateTextureImageView();
    CreateTextureSampler();
    
    m_descriptor.sampler = m_sampler;
    m_descriptor.imageView = m_view;
    m_descriptor.imageLayout = m_imageLayout;

    for(int i = 0; i < 6; i++) {
        stbi_image_free(imageData[i]);
    }
}

void Texture3D::CreateTextureImage(float const* const imageData[]) {
    m_mipLevels = static_cast<uint32_t>(floor(log2(std::max(m_width, m_height))) + 1);
    VkDeviceSize imageSize = m_faceSize * 6;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    m_renderer->CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(m_renderer->m_device, stagingBufferMemory, 0, imageSize, 0, &data);
        size_t offset = 0;
        for(int i = 0; i < 6; i++) {
            memcpy(static_cast<uint8_t*>(data) + offset, imageData[i], m_faceSize);
            offset += m_faceSize;
        }
    vkUnmapMemory(m_renderer->m_device, stagingBufferMemory);

    m_renderer->CreateImageCube(m_width, m_height, m_mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_deviceMemory);

    m_renderer->TransitionImageLayout(m_image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels, 6);
    m_renderer->CopyBufferToImageCube(stagingBuffer, m_image, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), m_mipLevels, m_faceSize);
    //renderer->transitionImageLayout(image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, 6);

    vkDestroyBuffer(m_renderer->m_device, stagingBuffer, nullptr);
    vkFreeMemory(m_renderer->m_device, stagingBufferMemory, nullptr);

    m_renderer->GenerateMipmapsCube(m_image, VK_FORMAT_R32G32B32A32_SFLOAT, m_width, m_height, m_mipLevels);
}

void Texture3D::CreateTextureImageView() {
    m_view = m_renderer->CreateImageViewCube(m_image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels);
}

void Texture3D::CreateTextureSampler() {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = static_cast<float>(m_mipLevels);
    samplerInfo.mipLodBias = 0;

    if (vkCreateSampler(m_renderer->m_device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }   
}

void Texture3D::Destroy() {
    vkDestroyImageView(m_renderer->m_device, m_view, nullptr);
    vkDestroyImage(m_renderer->m_device, m_image, nullptr);
    vkFreeMemory(m_renderer->m_device, m_deviceMemory, nullptr);
    vkDestroySampler(m_renderer->m_device, m_sampler, nullptr);
}


Primitive::Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material* material) : m_firstIndex(firstIndex), m_indexCount(indexCount), m_vertexCount(vertexCount), m_material(material) {
    m_hasIndices = indexCount > 0;
};

void Primitive::SetBoundingBox(glm::vec3 const& min, glm::vec3 const& max) {
    m_boundingBox.m_min = min;
    m_boundingBox.m_max = max;
    m_boundingBox.m_valid = true;
}

Mesh::Mesh(Renderer* renderer, glm::mat4 const& matrix) {
    this->m_renderer = renderer;
    this->m_uniformBlock.m_matrix = matrix;
    
    renderer->CreateBuffer(
        sizeof(m_uniformBlock),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        m_uniformBuffer.m_buffer,
        m_uniformBuffer.m_memory
    );

    vkMapMemory(renderer->m_device, m_uniformBuffer.m_memory, 0, sizeof(m_uniformBlock), 0, &m_uniformBuffer.m_mapped);
    memcpy(m_uniformBuffer.m_mapped, &m_uniformBlock, sizeof(m_uniformBlock));

    m_uniformBuffer.m_descriptor = { m_uniformBuffer.m_buffer, 0, sizeof(m_uniformBlock) };
};

Mesh::~Mesh() {
    vkDestroyBuffer(m_renderer->m_device, m_uniformBuffer.m_buffer, nullptr);
    vkFreeMemory(m_renderer->m_device, m_uniformBuffer.m_memory, nullptr);
    
    for (Primitive* p : m_primitives)
        delete p;
}

void Mesh::SetBoundingBox(glm::vec3 const& min, glm::vec3 const& max) {
    m_boundingBox.m_min = min;
    m_boundingBox.m_max = max;
    m_boundingBox.m_valid = true;
}


glm::mat4 Node::GetLocalMatrix() {
    return glm::translate(glm::mat4(1.0f), m_translation) * glm::mat4(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale) * m_matrix;
}

glm::mat4 Node::GetWorldMatrix() {
    glm::mat4 m = GetLocalMatrix();
    
    Node *p = m_parent;
    while (p) {
        m = p->GetLocalMatrix() * m;
        p = p->m_parent;
    }
    
    return m;
}

void Node::Update() {
    if (m_mesh) {
        m_mesh->m_uniformBlock.m_matrix = GetWorldMatrix();
        memcpy(m_mesh->m_uniformBuffer.m_mapped, &m_mesh->m_uniformBlock, sizeof(m_mesh->m_uniformBlock));
    }

    for (auto& child : m_children) {
        child->Update();
    }
}

Node::~Node() {
    if (m_mesh) {
        delete m_mesh;
    }

    for (auto& child : m_children) {
        delete child;
    }
}


void Model::Destroy()
{
    if (m_vertices.m_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_renderer->m_device, m_vertices.m_buffer, nullptr);
        vkFreeMemory(m_renderer->m_device, m_vertices.m_memory, nullptr);
    }

    if (m_indices.m_buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(m_renderer->m_device, m_indices.m_buffer, nullptr);
        vkFreeMemory(m_renderer->m_device, m_indices.m_memory, nullptr);
    }

    for (auto texture : m_textures) {
        texture.Destroy();
    }

    m_textures.resize(0);
    m_textureSamplers.resize(0);

    for (auto node : m_nodes) {
        delete node;
    }

    m_materials.resize(0);
    m_nodes.resize(0);
    m_linearNodes.resize(0);
};

void Model::LoadNode(Node* parent, tinygltf::Node const& node, uint32_t nodeIndex, tinygltf::Model const& model, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer)
{
    Node *newNode = new Node{};
    newNode->m_index = nodeIndex;
    newNode->m_parent = parent;
    newNode->m_matrix = glm::mat4(1.0f);

    // Generate local node matrix
    glm::vec3 translation = glm::vec3(0.0f);
    if (node.translation.size() == 3) {
        translation = glm::make_vec3(node.translation.data());
        newNode->m_translation = translation;
    }
    if (node.rotation.size() == 4) {
        glm::quat q = glm::make_quat(node.rotation.data());
        newNode->m_rotation = glm::mat4(q);
    }
    glm::vec3 scale = glm::vec3(1.0f);
    if (node.scale.size() == 3) {
        scale = glm::make_vec3(node.scale.data());
        newNode->m_scale = scale;
    }
    if (node.matrix.size() == 16) {
        newNode->m_matrix = glm::make_mat4x4(node.matrix.data());
    };

    // Node with children
    if (node.children.size() > 0) {
        for (size_t i = 0; i < node.children.size(); i++) {
            LoadNode(newNode, model.nodes[node.children[i]], node.children[i], model, indexBuffer, vertexBuffer);
        }
    }

    // Node contains mesh data
    if (node.mesh > -1) {
        const tinygltf::Mesh mesh = model.meshes[node.mesh];
        Mesh *newMesh = new Mesh(m_renderer, newNode->m_matrix);
        for (size_t j = 0; j < mesh.primitives.size(); j++) {
            const tinygltf::Primitive &primitive = mesh.primitives[j];
            uint32_t indexStart = static_cast<uint32_t>(indexBuffer.size());
            uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
            uint32_t indexCount = 0;
            uint32_t vertexCount = 0;
            glm::vec3 posMin{};
            glm::vec3 posMax{};
            bool hasIndices = primitive.indices > -1;
            
            // Vertices
            {
                const float *bufferPos = nullptr;
                const float *bufferNormals = nullptr;
                const float *bufferTexCoordSet0 = nullptr;
                const float *bufferTexCoordSet1 = nullptr;

                // Position attribute is required
                assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

                const tinygltf::Accessor &posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                const tinygltf::BufferView &posView = model.bufferViews[posAccessor.bufferView];
                bufferPos = reinterpret_cast<const float *>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
                posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
                posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
                vertexCount = static_cast<uint32_t>(posAccessor.count);

                if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
                    const tinygltf::Accessor &normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                    const tinygltf::BufferView &normView = model.bufferViews[normAccessor.bufferView];
                    bufferNormals = reinterpret_cast<const float *>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                }

                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
                    const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                    const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
                    bufferTexCoordSet0 = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                }
                if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end()) {
                    const tinygltf::Accessor &uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
                    const tinygltf::BufferView &uvView = model.bufferViews[uvAccessor.bufferView];
                    bufferTexCoordSet1 = reinterpret_cast<const float *>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                }

                for (size_t v = 0; v < posAccessor.count; v++) {
                    Vertex vert{};
                    vert.m_position = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
                    vert.m_normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
                    vert.m_uvSet0 = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * 2]) : glm::vec3(0.0f);
                    vert.m_uvSet1 = bufferTexCoordSet1 ? glm::make_vec2(&bufferTexCoordSet1[v * 2]) : glm::vec3(0.0f);

                    vertexBuffer.push_back(vert);
                }
            }

            // Indices
            if (hasIndices)
            {
                const tinygltf::Accessor &accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
                const tinygltf::BufferView &bufferView = model.bufferViews[accessor.bufferView];
                const tinygltf::Buffer &buffer = model.buffers[bufferView.buffer];

                indexCount = static_cast<uint32_t>(accessor.count);
                const void *dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

                switch (accessor.componentType) {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                    const uint32_t *buf = static_cast<const uint32_t*>(dataPtr);
                    for (size_t index = 0; index < accessor.count; index++) {
                        indexBuffer.push_back(buf[index] + vertexStart);
                    }
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                    const uint16_t *buf = static_cast<const uint16_t*>(dataPtr);
                    for (size_t index = 0; index < accessor.count; index++) {
                        indexBuffer.push_back(buf[index] + vertexStart);
                    }
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                    const uint8_t *buf = static_cast<const uint8_t*>(dataPtr);
                    for (size_t index = 0; index < accessor.count; index++) {
                        indexBuffer.push_back(buf[index] + vertexStart);
                    }
                    break;
                }
                default:
                    std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                    return;
                }
            }

            Primitive *newPrimitive = new Primitive(indexStart, indexCount, vertexCount, primitive.material > -1 ? &m_materials[primitive.material] : &m_materials.back());
            newPrimitive->SetBoundingBox(posMin, posMax);
            newMesh->m_primitives.push_back(newPrimitive);
        }

        // Mesh BB from BBs of primitives
        for (auto p : newMesh->m_primitives) {
            if (p->m_boundingBox.m_valid && !newMesh->m_boundingBox.m_valid) {
                newMesh->m_boundingBox = p->m_boundingBox;
                newMesh->m_boundingBox.m_valid = true;
            }
            newMesh->m_boundingBox.m_min = glm::min(newMesh->m_boundingBox.m_min, p->m_boundingBox.m_min);
            newMesh->m_boundingBox.m_max = glm::max(newMesh->m_boundingBox.m_max, p->m_boundingBox.m_max);
        }
        newNode->m_mesh = newMesh;
    }

    if (parent) {
        parent->m_children.push_back(newNode);
    } else {
        m_nodes.push_back(newNode);
    }
    m_linearNodes.push_back(newNode);
}

void Model::LoadTextures(tinygltf::Model& gltfModel, Renderer* renderer)
{
    for (tinygltf::Texture& tex : gltfModel.textures) {
        tinygltf::Image image = gltfModel.images[tex.source];
        
        TextureSampler textureSampler;
        if (tex.sampler != -1) 
        {
            textureSampler = m_textureSamplers[tex.sampler];
        }

        Texture2D texture;
        texture.FromglTfImage(image, textureSampler, renderer);
        m_textures.push_back(texture);
    }
}

VkSamplerAddressMode Model::GetVkWrapMode(int32_t wrapMode) 
{
    switch (wrapMode) {
    case 10497:
        return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    case 33071:
        return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    case 33648:
        return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
    default:
        return VK_SAMPLER_ADDRESS_MODE_MAX_ENUM;
    }
}

VkFilter Model::GetVkFilterMode(int32_t filterMode) 
{
    switch (filterMode) {
    case 9728:
        return VK_FILTER_NEAREST;
    case 9729:
        return VK_FILTER_LINEAR;
    case 9984:
        return VK_FILTER_NEAREST;
    case 9985:
        return VK_FILTER_NEAREST;
    case 9986:
        return VK_FILTER_LINEAR;
    case 9987:
        return VK_FILTER_LINEAR;
    default:
        return VK_FILTER_MAX_ENUM;
    }
}

void Model::LoadTextureSamplers(tinygltf::Model const& gltfModel)
{
    for (tinygltf::Sampler smpl : gltfModel.samplers) {
        TextureSampler sampler{};
        sampler.m_minFilter = GetVkFilterMode(smpl.minFilter);
        sampler.m_magFilter = GetVkFilterMode(smpl.magFilter);
        sampler.m_addressModeU = GetVkWrapMode(smpl.wrapS);
        sampler.m_addressModeV = GetVkWrapMode(smpl.wrapT);
        sampler.m_addressModeW = sampler.m_addressModeV;
        m_textureSamplers.push_back(sampler);
    }
}

void Model::LoadMaterials(tinygltf::Model& gltfModel)
{
    for (tinygltf::Material& mat : gltfModel.materials) {
        Material material{};
        if (mat.values.find("baseColorTexture") != mat.values.end()) {
            material.m_baseColorTexture = &m_textures[mat.values["baseColorTexture"].TextureIndex()];
            material.m_baseColorTextCoordSet = mat.values["baseColorTexture"].TextureTexCoord();
        }
        if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
            material.m_metallicRoughnessTexture = &m_textures[mat.values["metallicRoughnessTexture"].TextureIndex()];
            material.m_metallicRoughnessTextCoordSet = mat.values["metallicRoughnessTexture"].TextureTexCoord();
        }
        if (mat.values.find("roughnessFactor") != mat.values.end()) {
            material.m_roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
        }
        if (mat.values.find("metallicFactor") != mat.values.end()) {
            material.m_metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
        }
        if (mat.values.find("baseColorFactor") != mat.values.end()) {
            material.m_baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
        }				
        if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
            material.m_normalTexture = &m_textures[mat.additionalValues["normalTexture"].TextureIndex()];
            material.m_normalTextCoordSet = mat.additionalValues["normalTexture"].TextureTexCoord();
        }
        if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
            material.m_emissiveTexture = &m_textures[mat.additionalValues["emissiveTexture"].TextureIndex()];
            material.m_emissiveTextCoordSet = mat.additionalValues["emissiveTexture"].TextureTexCoord();
        }
        if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
            material.m_occlusionTexture = &m_textures[mat.additionalValues["occlusionTexture"].TextureIndex()];
            material.m_occlusionTextCoordSet = mat.additionalValues["occlusionTexture"].TextureTexCoord();
        }
        if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
            tinygltf::Parameter param = mat.additionalValues["alphaMode"];
            if (param.string_value == "BLEND") {
                material.m_alphaMode = Material::ALPHAMODE_BLEND;
            }
            if (param.string_value == "MASK") {
                material.m_alphaCutoff = 0.5f;
                material.m_alphaMode = Material::ALPHAMODE_MASK;
            }
        }
        if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
            material.m_alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
        }
        if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end()) {
            material.m_emissiveFactor = glm::vec4(glm::make_vec3(mat.additionalValues["emissiveFactor"].ColorFactor().data()), 1.0);
            material.m_emissiveFactor = glm::vec4(0.0f);
        }

        m_materials.push_back(material);
    }

    // Push a default material at the end of the list for meshes with no material assigned
    m_materials.push_back(Material());
}

void Model::LoadFromFile(std::string const filename, Renderer* renderer)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    std::string error;
    std::string warning;

    this->m_renderer = renderer;

    bool binary = false;
    size_t extpos = filename.rfind('.', filename.length());
    if (extpos != std::string::npos) {
        binary = (filename.substr(extpos + 1, filename.length() - extpos) == "glb");
    }

    bool fileLoaded = binary ? 
        gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, filename.c_str()) :
        gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename.c_str());

    std::vector<Vertex> vertexBuffer;
    std::vector<uint32_t> indexBuffer;

    if (fileLoaded) {
        LoadTextureSamplers(gltfModel);
        LoadTextures(gltfModel, renderer);
        LoadMaterials(gltfModel);

        const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
            LoadNode(nullptr, node, scene.nodes[i], gltfModel, indexBuffer, vertexBuffer);
        }

        for (auto node : m_linearNodes) {
            // Initial pose
            if (node->m_mesh) {
                node->Update();
            }
        }
    } else {
        std::cerr << "Could not load gltf file: " << error << std::endl;
        return;
    }

    m_indices.m_count = static_cast<uint32_t>(indexBuffer.size());

    CreateVertexBuffer(vertexBuffer);
    if (m_indices.m_count > 0) { 
        CreateIndexBuffer(indexBuffer);
    }

    GetSceneDimensions();
}

void Model::DrawNode(Node const* node, VkCommandBuffer commandBuffer)
{
    if (node->m_mesh) {
        for (Primitive *primitive : node->m_mesh->m_primitives) {
            vkCmdDrawIndexed(commandBuffer, primitive->m_indexCount, 1, primitive->m_firstIndex, 0, 0);
        }
    }
    for (auto& child : node->m_children) {
        DrawNode(child, commandBuffer);
    }
}

void Model::Draw(VkCommandBuffer commandBuffer)
{
    const VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_vertices.m_buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_indices.m_buffer, 0, VK_INDEX_TYPE_UINT32);
    for (auto& node : m_nodes) {
        DrawNode(node, commandBuffer);
    }
}

void Model::CalculateBoundingBox(Node* node, Node const* parent) {
    BoundingBox parentBvh = parent ? parent->m_boundingBox : BoundingBox(m_dimensions.m_min, m_dimensions.m_max);

    if (node->m_mesh) {
        if (node->m_mesh->m_boundingBox.m_valid) {
            node->m_boundingBox2 = node->m_mesh->m_boundingBox.FromMatrix(node->GetWorldMatrix());
            if (node->m_children.size() == 0) {
                node->m_boundingBox.m_min = node->m_boundingBox2.m_min;
                node->m_boundingBox.m_max = node->m_boundingBox2.m_max;
                node->m_boundingBox.m_valid = true;
            }
        }
    }

    parentBvh.m_min = glm::min(parentBvh.m_min, node->m_boundingBox.m_min);
    parentBvh.m_max = glm::min(parentBvh.m_max, node->m_boundingBox.m_max);

    for (auto &child : node->m_children) {
        CalculateBoundingBox(child, node);
    }
}

void Model::GetSceneDimensions()
{
    // Calculate binary volume hierarchy for all nodes in the scene
    for (auto node : m_linearNodes) {
        CalculateBoundingBox(node, nullptr);
    }

    m_dimensions.m_min = glm::vec3(FLT_MAX);
    m_dimensions.m_max = glm::vec3(-FLT_MAX);

    for (auto node : m_linearNodes) {
        if (node->m_boundingBox.m_valid) {
            m_dimensions.m_min = glm::min(m_dimensions.m_min, node->m_boundingBox.m_min);
            m_dimensions.m_max = glm::max(m_dimensions.m_max, node->m_boundingBox.m_max);
        }
    }

    // Calculate scene aabb
    m_boundingBox = glm::scale(glm::mat4(1.0f), glm::vec3(m_dimensions.m_max[0] - m_dimensions.m_min[0], m_dimensions.m_max[1] - m_dimensions.m_min[1], m_dimensions.m_max[2] - m_dimensions.m_min[2]));
    m_boundingBox[3][0] = m_dimensions.m_min[0];
    m_boundingBox[3][1] = m_dimensions.m_min[1];
    m_boundingBox[3][2] = m_dimensions.m_min[2];
}

void Model::CreateVertexBuffer(std::vector<Vertex> const& vertices) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    m_renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(m_renderer->m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(m_renderer->m_device, stagingBufferMemory);

    m_renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->m_vertices.m_buffer, this->m_vertices.m_memory);

    m_renderer->CopyBuffer(stagingBuffer, this->m_vertices.m_buffer, bufferSize);

    vkDestroyBuffer(m_renderer->m_device, stagingBuffer, nullptr);
    vkFreeMemory(m_renderer->m_device, stagingBufferMemory, nullptr);
}

void Model::CreateIndexBuffer(std::vector<uint32_t> const& indices) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    m_renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(m_renderer->m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(m_renderer->m_device, stagingBufferMemory);

    m_renderer->CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->m_indices.m_buffer, this->m_indices.m_memory);

    m_renderer->CopyBuffer(stagingBuffer, this->m_indices.m_buffer, bufferSize);

    vkDestroyBuffer(m_renderer->m_device, stagingBuffer, nullptr);
    vkFreeMemory(m_renderer->m_device, stagingBufferMemory, nullptr);
}

VkVertexInputBindingDescription Model::Vertex::GetBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 4> Model::Vertex::GetAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, m_position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, m_normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, m_uvSet0);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, m_uvSet1);

    return attributeDescriptions;
}