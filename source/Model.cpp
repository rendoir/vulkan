#include "Model.hpp"
#include "Renderer.hpp"

#define TINYGLTF_IMPLEMENTATION
#define TINYGLTF_NO_STB_IMAGE_WRITE
#define STB_IMAGE_IMPLEMENTATION
#include <glTF/tiny_gltf.h>

#include <iostream>


const TextureSampler TextureSampler::defaultSampler = {
    VK_FILTER_LINEAR,
    VK_FILTER_LINEAR,
    VK_SAMPLER_ADDRESS_MODE_REPEAT,
    VK_SAMPLER_ADDRESS_MODE_REPEAT,
    VK_SAMPLER_ADDRESS_MODE_REPEAT
};

BoundingBox::BoundingBox() {};

BoundingBox::BoundingBox(glm::vec3 min, glm::vec3 max) 
    : min(min), max(max) {}

BoundingBox BoundingBox::getAABB(glm::mat4 m) {
    glm::vec3 min = glm::vec3(m[3]);
    glm::vec3 max = min;
    glm::vec3 v0, v1;
    
    glm::vec3 right = glm::vec3(m[0]);
    v0 = right * this->min.x;
    v1 = right * this->max.x;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    glm::vec3 up = glm::vec3(m[1]);
    v0 = up * this->min.y;
    v1 = up * this->max.y;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    glm::vec3 back = glm::vec3(m[2]);
    v0 = back * this->min.z;
    v1 = back * this->max.z;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    return BoundingBox(min, max);
}


void Texture::fromglTfImage(tinygltf::Image &gltfimage, TextureSampler textureSampler, Renderer* renderer) {
    this->renderer = renderer;

    unsigned char* imageData = gltfimage.image.data();
    VkDeviceSize imageSize = gltfimage.image.size();
    width = gltfimage.width;
    height = gltfimage.height;
    imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO - Make parameter

    createTextureImage(imageData, imageSize);
    createTextureImageView();
    createTextureSampler(textureSampler);
    
    descriptor.sampler = sampler;
    descriptor.imageView = view;
    descriptor.imageLayout = imageLayout;
}

void Texture::fromFile(std::string filename, Renderer *renderer) {
    this->renderer = renderer;

    int channels;
    stbi_uc* imageData = stbi_load(filename.c_str(), (int*)&width, (int*)&height, &channels, STBI_rgb_alpha);
    VkDeviceSize imageSize = width * height * STBI_rgb_alpha;
    imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO - Make parameter

    if (!imageData) {
        throw std::runtime_error("failed to load texture image!");
    }

    createTextureImage(imageData, imageSize);
    createTextureImageView();
    createTextureSampler(TextureSampler::defaultSampler);

    descriptor.sampler = sampler;
    descriptor.imageView = view;
    descriptor.imageLayout = imageLayout;

    stbi_image_free(imageData);
}

void Texture::createTextureImage(unsigned char* imageData, VkDeviceSize imageSize) {
    mipLevels = static_cast<uint32_t>(floor(log2(std::max(width, height))) + 1);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    renderer->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer->device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, imageData, static_cast<size_t>(imageSize));
    vkUnmapMemory(renderer->device, stagingBufferMemory);

    renderer->createImage(width, height, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, deviceMemory);

    renderer->transitionImageLayout(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels);
    renderer->copyBufferToImage(stagingBuffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height));
    //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

    vkDestroyBuffer(renderer->device, stagingBuffer, nullptr);
    vkFreeMemory(renderer->device, stagingBufferMemory, nullptr);

    renderer->generateMipmaps(image, VK_FORMAT_R8G8B8A8_UNORM, width, height, mipLevels);
}

void Texture::createTextureImageView() {
    view = renderer->createImageView(image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}

void Texture::createTextureSampler(TextureSampler textureSampler) {
    VkSamplerCreateInfo samplerInfo{};
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = textureSampler.magFilter;
    samplerInfo.minFilter = textureSampler.minFilter;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.addressModeU = textureSampler.addressModeU;
    samplerInfo.addressModeV = textureSampler.addressModeV;
    samplerInfo.addressModeW = textureSampler.addressModeW;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_NEVER;
    samplerInfo.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
    samplerInfo.maxAnisotropy = 16;
    samplerInfo.anisotropyEnable = VK_TRUE;
    samplerInfo.minLod = 0;
    samplerInfo.maxLod = static_cast<float>(mipLevels);
    samplerInfo.mipLodBias = 0;

    if (vkCreateSampler(renderer->device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }   
}

void Texture::destroy()
{
    vkDestroyImageView(renderer->device, view, nullptr);
    vkDestroyImage(renderer->device, image, nullptr);
    vkFreeMemory(renderer->device, deviceMemory, nullptr);
    vkDestroySampler(renderer->device, sampler, nullptr);
}


void Texture3D::fromFolder(std::string folderName, Renderer *renderer) {
    this->renderer = renderer;
    std::string fileNames[] = { "negx.hdr", "posx.hdr", "negy.hdr", "posy.hdr", "negz.hdr", "posz.hdr" }; // TODO - Should be a parameter?
    
    float *imageData[6];
    for(int i = 0; i < 6; i++) {
        int channels;
        imageData[i] = stbi_loadf((folderName + '/' + fileNames[i]).c_str(), (int*)&width, (int*)&height, &channels, STBI_rgb_alpha);

        if(!imageData[i]) {
            std::cerr << "Error loading image: " << folderName  << std::endl;
            assert(imageData[i] != nullptr);
        }
    }

    imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO - Make parameter
    faceSize = width * height * STBI_rgb_alpha * 4;

    createTextureImage(imageData);
    createTextureImageView();
    createTextureSampler();
    
    descriptor.sampler = sampler;
    descriptor.imageView = view;
    descriptor.imageLayout = imageLayout;

    for(int i = 0; i < 6; i++) {
        stbi_image_free(imageData[i]);
    }
}

void Texture3D::createTextureImage(float *imageData[]) {
    mipLevels = static_cast<uint32_t>(floor(log2(std::max(width, height))) + 1);
    VkDeviceSize imageSize = faceSize * 6;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    renderer->createBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer->device, stagingBufferMemory, 0, imageSize, 0, &data);
        size_t offset = 0;
        for(int i = 0; i < 6; i++) {
            memcpy(static_cast<uint8_t*>(data) + offset, imageData[i], faceSize);
            offset += faceSize;
        }
    vkUnmapMemory(renderer->device, stagingBufferMemory);

    renderer->createImageCube(width, height, mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, image, deviceMemory);

    renderer->transitionImageLayout(image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, mipLevels, 6);
    renderer->copyBufferToImageCube(stagingBuffer, image, static_cast<uint32_t>(width), static_cast<uint32_t>(height), mipLevels, faceSize);
    //renderer->transitionImageLayout(image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, 6);

    vkDestroyBuffer(renderer->device, stagingBuffer, nullptr);
    vkFreeMemory(renderer->device, stagingBufferMemory, nullptr);

    renderer->generateMipmapsCube(image, VK_FORMAT_R32G32B32A32_SFLOAT, width, height, mipLevels);
}

void Texture3D::createTextureImageView() {
    view = renderer->createImageViewCube(image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, mipLevels);
}

void Texture3D::createTextureSampler() {
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
    samplerInfo.maxLod = static_cast<float>(mipLevels);
    samplerInfo.mipLodBias = 0;

    if (vkCreateSampler(renderer->device, &samplerInfo, nullptr, &sampler) != VK_SUCCESS) {
        throw std::runtime_error("failed to create texture sampler!");
    }   
}

void Texture3D::destroy() {
    vkDestroyImageView(renderer->device, view, nullptr);
    vkDestroyImage(renderer->device, image, nullptr);
    vkFreeMemory(renderer->device, deviceMemory, nullptr);
    vkDestroySampler(renderer->device, sampler, nullptr);
}


Primitive::Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material &material) : firstIndex(firstIndex), indexCount(indexCount), vertexCount(vertexCount), material(material) {
    hasIndices = indexCount > 0;
};

void Primitive::setBoundingBox(glm::vec3 min, glm::vec3 max) {
    bb.min = min;
    bb.max = max;
    bb.valid = true;
}

Mesh::Mesh(Renderer *renderer, glm::mat4 matrix) {
    this->renderer = renderer;
    this->uniformBlock.matrix = matrix;
    
    renderer->createBuffer(
        sizeof(uniformBlock),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        uniformBuffer.buffer,
        uniformBuffer.memory
    );

    vkMapMemory(renderer->device, uniformBuffer.memory, 0, sizeof(uniformBlock), 0, &uniformBuffer.mapped);
    memcpy(uniformBuffer.mapped, &uniformBlock, sizeof(uniformBlock));

    uniformBuffer.descriptor = { uniformBuffer.buffer, 0, sizeof(uniformBlock) };
};

Mesh::~Mesh() {
    vkDestroyBuffer(renderer->device, uniformBuffer.buffer, nullptr);
    vkFreeMemory(renderer->device, uniformBuffer.memory, nullptr);
    
    for (Primitive* p : primitives)
        delete p;
}

void Mesh::setBoundingBox(glm::vec3 min, glm::vec3 max) {
    bb.min = min;
    bb.max = max;
    bb.valid = true;
}


glm::mat4 Node::localMatrix() {
    return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * matrix;
}

glm::mat4 Node::getMatrix() {
    glm::mat4 m = localMatrix();
    
    Node *p = parent;
    while (p) {
        m = p->localMatrix() * m;
        p = p->parent;
    }
    
    return m;
}

void Node::update() {
    if (mesh) {
        mesh->uniformBlock.matrix = getMatrix();
        memcpy(mesh->uniformBuffer.mapped, &mesh->uniformBlock, sizeof(mesh->uniformBlock));
    }

    for (auto& child : children) {
        child->update();
    }
}

Node::~Node() {
    if (mesh) {
        delete mesh;
    }

    for (auto& child : children) {
        delete child;
    }
}


void Model::destroy()
{
    if (vertices.buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(renderer->device, vertices.buffer, nullptr);
        vkFreeMemory(renderer->device, vertices.memory, nullptr);
    }

    if (indices.buffer != VK_NULL_HANDLE) {
        vkDestroyBuffer(renderer->device, indices.buffer, nullptr);
        vkFreeMemory(renderer->device, indices.memory, nullptr);
    }

    for (auto texture : textures) {
        texture.destroy();
    }

    textures.resize(0);
    textureSamplers.resize(0);

    for (auto node : nodes) {
        delete node;
    }

    materials.resize(0);
    nodes.resize(0);
    linearNodes.resize(0);
};

void Model::loadNode(Node *parent, const tinygltf::Node &node, uint32_t nodeIndex, const tinygltf::Model &model, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer, float globalscale)
{
    Node *newNode = new Node{};
    newNode->index = nodeIndex;
    newNode->parent = parent;
    newNode->name = node.name;
    newNode->matrix = glm::mat4(1.0f);

    // Generate local node matrix
    glm::vec3 translation = glm::vec3(0.0f);
    if (node.translation.size() == 3) {
        translation = glm::make_vec3(node.translation.data());
        newNode->translation = translation;
    }
    if (node.rotation.size() == 4) {
        glm::quat q = glm::make_quat(node.rotation.data());
        newNode->rotation = glm::mat4(q);
    }
    glm::vec3 scale = glm::vec3(1.0f);
    if (node.scale.size() == 3) {
        scale = glm::make_vec3(node.scale.data());
        newNode->scale = scale;
    }
    if (node.matrix.size() == 16) {
        newNode->matrix = glm::make_mat4x4(node.matrix.data());
    };

    // Node with children
    if (node.children.size() > 0) {
        for (size_t i = 0; i < node.children.size(); i++) {
            loadNode(newNode, model.nodes[node.children[i]], node.children[i], model, indexBuffer, vertexBuffer, globalscale);
        }
    }

    // Node contains mesh data
    if (node.mesh > -1) {
        const tinygltf::Mesh mesh = model.meshes[node.mesh];
        Mesh *newMesh = new Mesh(renderer, newNode->matrix);
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
                    vert.position = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
                    vert.normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
                    vert.uv0 = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * 2]) : glm::vec3(0.0f);
                    vert.uv1 = bufferTexCoordSet1 ? glm::make_vec2(&bufferTexCoordSet1[v * 2]) : glm::vec3(0.0f);

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

            Primitive *newPrimitive = new Primitive(indexStart, indexCount, vertexCount, primitive.material > -1 ? materials[primitive.material] : materials.back());
            newPrimitive->setBoundingBox(posMin, posMax);
            newMesh->primitives.push_back(newPrimitive);
        }

        // Mesh BB from BBs of primitives
        for (auto p : newMesh->primitives) {
            if (p->bb.valid && !newMesh->bb.valid) {
                newMesh->bb = p->bb;
                newMesh->bb.valid = true;
            }
            newMesh->bb.min = glm::min(newMesh->bb.min, p->bb.min);
            newMesh->bb.max = glm::max(newMesh->bb.max, p->bb.max);
        }
        newNode->mesh = newMesh;
    }

    if (parent) {
        parent->children.push_back(newNode);
    } else {
        nodes.push_back(newNode);
    }
    linearNodes.push_back(newNode);
}

void Model::loadTextures(tinygltf::Model &gltfModel, Renderer* renderer)
{
    for (tinygltf::Texture &tex : gltfModel.textures) {
        tinygltf::Image image = gltfModel.images[tex.source];
        
        TextureSampler textureSampler;
        if (tex.sampler == -1) {
            // No sampler specified, use a default one
            textureSampler = TextureSampler::defaultSampler;
        } else {
            textureSampler = textureSamplers[tex.sampler];
        }

        Texture texture;
        texture.fromglTfImage(image, textureSampler, renderer);
        textures.push_back(texture);
    }
}

VkSamplerAddressMode Model::getVkWrapMode(int32_t wrapMode) 
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

VkFilter Model::getVkFilterMode(int32_t filterMode) 
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

void Model::loadTextureSamplers(tinygltf::Model &gltfModel)
{
    for (tinygltf::Sampler smpl : gltfModel.samplers) {
        TextureSampler sampler{};
        sampler.minFilter = getVkFilterMode(smpl.minFilter);
        sampler.magFilter = getVkFilterMode(smpl.magFilter);
        sampler.addressModeU = getVkWrapMode(smpl.wrapS);
        sampler.addressModeV = getVkWrapMode(smpl.wrapT);
        sampler.addressModeW = sampler.addressModeV;
        textureSamplers.push_back(sampler);
    }
}

void Model::loadMaterials(tinygltf::Model &gltfModel)
{
    for (tinygltf::Material &mat : gltfModel.materials) {
        Material material{};
        if (mat.values.find("baseColorTexture") != mat.values.end()) {
            material.baseColorTexture = &textures[mat.values["baseColorTexture"].TextureIndex()];
            material.baseColorTextCoordSet = mat.values["baseColorTexture"].TextureTexCoord();
        }
        if (mat.values.find("metallicRoughnessTexture") != mat.values.end()) {
            material.metallicRoughnessTexture = &textures[mat.values["metallicRoughnessTexture"].TextureIndex()];
            material.metallicRoughnessTextCoordSet = mat.values["metallicRoughnessTexture"].TextureTexCoord();
        }
        if (mat.values.find("roughnessFactor") != mat.values.end()) {
            material.roughnessFactor = static_cast<float>(mat.values["roughnessFactor"].Factor());
        }
        if (mat.values.find("metallicFactor") != mat.values.end()) {
            material.metallicFactor = static_cast<float>(mat.values["metallicFactor"].Factor());
        }
        if (mat.values.find("baseColorFactor") != mat.values.end()) {
            material.baseColorFactor = glm::make_vec4(mat.values["baseColorFactor"].ColorFactor().data());
        }				
        if (mat.additionalValues.find("normalTexture") != mat.additionalValues.end()) {
            material.normalTexture = &textures[mat.additionalValues["normalTexture"].TextureIndex()];
            material.normalTextCoordSet = mat.additionalValues["normalTexture"].TextureTexCoord();
        }
        if (mat.additionalValues.find("emissiveTexture") != mat.additionalValues.end()) {
            material.emissiveTexture = &textures[mat.additionalValues["emissiveTexture"].TextureIndex()];
            material.emissiveTextCoordSet = mat.additionalValues["emissiveTexture"].TextureTexCoord();
        }
        if (mat.additionalValues.find("occlusionTexture") != mat.additionalValues.end()) {
            material.occlusionTexture = &textures[mat.additionalValues["occlusionTexture"].TextureIndex()];
            material.occlusionTextCoordSet = mat.additionalValues["occlusionTexture"].TextureTexCoord();
        }
        if (mat.additionalValues.find("alphaMode") != mat.additionalValues.end()) {
            tinygltf::Parameter param = mat.additionalValues["alphaMode"];
            if (param.string_value == "BLEND") {
                material.alphaMode = Material::ALPHAMODE_BLEND;
            }
            if (param.string_value == "MASK") {
                material.alphaCutoff = 0.5f;
                material.alphaMode = Material::ALPHAMODE_MASK;
            }
        }
        if (mat.additionalValues.find("alphaCutoff") != mat.additionalValues.end()) {
            material.alphaCutoff = static_cast<float>(mat.additionalValues["alphaCutoff"].Factor());
        }
        if (mat.additionalValues.find("emissiveFactor") != mat.additionalValues.end()) {
            material.emissiveFactor = glm::vec4(glm::make_vec3(mat.additionalValues["emissiveFactor"].ColorFactor().data()), 1.0);
            material.emissiveFactor = glm::vec4(0.0f);
        }

        materials.push_back(material);
    }

    // Push a default material at the end of the list for meshes with no material assigned
    materials.push_back(Material());
}

void Model::loadFromFile(std::string filename, Renderer *renderer, float scale /*= 1.0f*/)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    std::string error;
    std::string warning;

    this->renderer = renderer;

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
        loadTextureSamplers(gltfModel);
        loadTextures(gltfModel, renderer);
        loadMaterials(gltfModel);

        const tinygltf::Scene &scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
        for (size_t i = 0; i < scene.nodes.size(); i++) {
            const tinygltf::Node node = gltfModel.nodes[scene.nodes[i]];
            loadNode(nullptr, node, scene.nodes[i], gltfModel, indexBuffer, vertexBuffer, scale);
        }

        for (auto node : linearNodes) {
            // Initial pose
            if (node->mesh) {
                node->update();
            }
        }
    } else {
        std::cerr << "Could not load gltf file: " << error << std::endl;
        return;
    }

    indices.count = static_cast<uint32_t>(indexBuffer.size());

    createVertexBuffer(vertexBuffer);
    if (indices.count > 0) { 
        createIndexBuffer(indexBuffer);
    }

    getSceneDimensions();
}

void Model::drawNode(Node *node, VkCommandBuffer commandBuffer)
{
    if (node->mesh) {
        for (Primitive *primitive : node->mesh->primitives) {
            vkCmdDrawIndexed(commandBuffer, primitive->indexCount, 1, primitive->firstIndex, 0, 0);
        }
    }
    for (auto& child : node->children) {
        drawNode(child, commandBuffer);
    }
}

void Model::draw(VkCommandBuffer commandBuffer)
{
    const VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &vertices.buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, indices.buffer, 0, VK_INDEX_TYPE_UINT32);
    for (auto& node : nodes) {
        drawNode(node, commandBuffer);
    }
}

void Model::calculateBoundingBox(Node *node, Node *parent) {
    BoundingBox parentBvh = parent ? parent->bvh : BoundingBox(dimensions.min, dimensions.max);

    if (node->mesh) {
        if (node->mesh->bb.valid) {
            node->aabb = node->mesh->bb.getAABB(node->getMatrix());
            if (node->children.size() == 0) {
                node->bvh.min = node->aabb.min;
                node->bvh.max = node->aabb.max;
                node->bvh.valid = true;
            }
        }
    }

    parentBvh.min = glm::min(parentBvh.min, node->bvh.min);
    parentBvh.max = glm::min(parentBvh.max, node->bvh.max);

    for (auto &child : node->children) {
        calculateBoundingBox(child, node);
    }
}

void Model::getSceneDimensions()
{
    // Calculate binary volume hierarchy for all nodes in the scene
    for (auto node : linearNodes) {
        calculateBoundingBox(node, nullptr);
    }

    dimensions.min = glm::vec3(FLT_MAX);
    dimensions.max = glm::vec3(-FLT_MAX);

    for (auto node : linearNodes) {
        if (node->bvh.valid) {
            dimensions.min = glm::min(dimensions.min, node->bvh.min);
            dimensions.max = glm::max(dimensions.max, node->bvh.max);
        }
    }

    // Calculate scene aabb
    aabb = glm::scale(glm::mat4(1.0f), glm::vec3(dimensions.max[0] - dimensions.min[0], dimensions.max[1] - dimensions.min[1], dimensions.max[2] - dimensions.min[2]));
    aabb[3][0] = dimensions.min[0];
    aabb[3][1] = dimensions.min[1];
    aabb[3][2] = dimensions.min[2];
}

Node* Model::findNode(Node *parent, uint32_t index) {
    Node* nodeFound = nullptr;
    if (parent->index == index) {
        return parent;
    }
    for (auto& child : parent->children) {
        nodeFound = findNode(child, index);
        if (nodeFound) {
            break;
        }
    }
    return nodeFound;
}

Node* Model::nodeFromIndex(uint32_t index) {
    Node* nodeFound = nullptr;
    for (auto &node : nodes) {
        nodeFound = findNode(node, index);
        if (nodeFound) {
            break;
        }
    }
    return nodeFound;
}

void Model::createVertexBuffer(std::vector<Vertex> vertices) {
    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer->device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(renderer->device, stagingBufferMemory);

    renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->vertices.buffer, this->vertices.memory);

    renderer->copyBuffer(stagingBuffer, this->vertices.buffer, bufferSize);

    vkDestroyBuffer(renderer->device, stagingBuffer, nullptr);
    vkFreeMemory(renderer->device, stagingBufferMemory, nullptr);
}

void Model::createIndexBuffer(std::vector<uint32_t> indices) {
    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer->device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(renderer->device, stagingBufferMemory);

    renderer->createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, this->indices.buffer, this->indices.memory);

    renderer->copyBuffer(stagingBuffer, this->indices.buffer, bufferSize);

    vkDestroyBuffer(renderer->device, stagingBuffer, nullptr);
    vkFreeMemory(renderer->device, stagingBufferMemory, nullptr);
}

VkVertexInputBindingDescription Model::Vertex::getBindingDescription() {
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 4> Model::Vertex::getAttributeDescriptions() {
    std::array<VkVertexInputAttributeDescription, 4> attributeDescriptions = {};

    attributeDescriptions[0].binding = 0;
    attributeDescriptions[0].location = 0;
    attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[0].offset = offsetof(Vertex, position);

    attributeDescriptions[1].binding = 0;
    attributeDescriptions[1].location = 1;
    attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
    attributeDescriptions[1].offset = offsetof(Vertex, normal);

    attributeDescriptions[2].binding = 0;
    attributeDescriptions[2].location = 2;
    attributeDescriptions[2].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[2].offset = offsetof(Vertex, uv0);

    attributeDescriptions[3].binding = 0;
    attributeDescriptions[3].location = 3;
    attributeDescriptions[3].format = VK_FORMAT_R32G32_SFLOAT;
    attributeDescriptions[3].offset = offsetof(Vertex, uv1);

    return attributeDescriptions;
}