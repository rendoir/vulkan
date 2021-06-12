#include "Model.hpp"
#include "Renderer.hpp"


BoundingBox BoundingBox::FromMatrix(glm::mat4 const& matrix)
{
    glm::vec3 min = glm::vec3(matrix[3]);
    glm::vec3 max = min;
    glm::vec3 v0, v1;
    
    glm::vec3 right = glm::vec3(matrix[0]);
    v0 = right * m_min.x;
    v1 = right * m_max.x;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    glm::vec3 up = glm::vec3(matrix[1]);
    v0 = up * m_min.y;
    v1 = up * m_max.y;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    glm::vec3 back = glm::vec3(matrix[2]);
    v0 = back * m_min.z;
    v1 = back * m_max.z;
    min += glm::min(v0, v1);
    max += glm::max(v0, v1);

    return BoundingBox(min, max);
}


void Texture2D::FromglTfImage(tinygltf::Image const& gltfimage, TextureSampler const& textureSampler)
{
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

void Texture2D::FromFile(std::string const filename)
{
    int channels;
    stbi_uc* imageData = stbi_load(filename.c_str(), (int*)&m_width, (int*)&m_height, &channels, STBI_rgb_alpha);
    VkDeviceSize imageSize = m_width * m_height * STBI_rgb_alpha;
    m_imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // TODO - Make parameter

    if (!imageData)
    {
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

void Texture2D::CreateTextureImage(uint8_t const* const imageData, VkDeviceSize const& imageSize)
{
    Renderer& renderer = Renderer::GetInstance();

    m_mipLevels = static_cast<uint32_t>(floor(log2(std::max(m_width, m_height))) + 1);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    renderer.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer.m_device, stagingBufferMemory, 0, imageSize, 0, &data);
        memcpy(data, imageData, static_cast<size_t>(imageSize));
    vkUnmapMemory(renderer.m_device, stagingBufferMemory);

    renderer.CreateImage(m_width, m_height, m_mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_deviceMemory);

    renderer.TransitionImageLayout(m_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels);
    renderer.CopyBufferToImage(stagingBuffer, m_image, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height));
    //transitioned to VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL while generating mipmaps

    vkDestroyBuffer(renderer.m_device, stagingBuffer, nullptr);
    vkFreeMemory(renderer.m_device, stagingBufferMemory, nullptr);

    renderer.GenerateMipmaps(m_image, VK_FORMAT_R8G8B8A8_UNORM, m_width, m_height, m_mipLevels);
}

void Texture2D::CreateTextureImageView()
{
    m_view = Renderer::GetInstance().CreateImageView(m_image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels);
}

void Texture2D::CreateTextureSampler(TextureSampler const& textureSampler)
{
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

    if (vkCreateSampler(Renderer::GetInstance().m_device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }   
}

void Texture2D::Destroy()
{
    Renderer& renderer = Renderer::GetInstance();
    vkDestroyImageView(renderer.m_device, m_view, nullptr);
    vkDestroyImage(renderer.m_device, m_image, nullptr);
    vkFreeMemory(renderer.m_device, m_deviceMemory, nullptr);
    vkDestroySampler(renderer.m_device, m_sampler, nullptr);
}


void Texture3D::FromFolder(std::string const folderName)
{
    std::string fileNames[] = { "negx.hdr", "posx.hdr", "negy.hdr", "posy.hdr", "negz.hdr", "posz.hdr" }; // TODO - Should be a parameter?
    
    float* imageData[6];
    for(int i = 0; i < 6; i++)
    {
        int channels;
        imageData[i] = stbi_loadf((folderName + '/' + fileNames[i]).c_str(), (int*)&m_width, (int*)&m_height, &channels, STBI_rgb_alpha);

        if(!imageData[i])
        {
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

    for(int i = 0; i < 6; i++)
    {
        stbi_image_free(imageData[i]);
    }
}

void Texture3D::CreateTextureImage(float const* const imageData[])
{
	Renderer& renderer = Renderer::GetInstance();

    m_mipLevels = static_cast<uint32_t>(floor(log2(std::max(m_width, m_height))) + 1);
    VkDeviceSize imageSize = m_faceSize * 6;

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    renderer.CreateBuffer(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer.m_device, stagingBufferMemory, 0, imageSize, 0, &data);
        size_t offset = 0;
        for(int i = 0; i < 6; i++)
        {
            memcpy(static_cast<uint8_t*>(data) + offset, imageData[i], m_faceSize);
            offset += m_faceSize;
        }
    vkUnmapMemory(renderer.m_device, stagingBufferMemory);

    renderer.CreateImageCube(m_width, m_height, m_mipLevels, VK_SAMPLE_COUNT_1_BIT, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_image, m_deviceMemory);

    renderer.TransitionImageLayout(m_image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels, 6);
    renderer.CopyBufferToImageCube(stagingBuffer, m_image, static_cast<uint32_t>(m_width), static_cast<uint32_t>(m_height), m_mipLevels, m_faceSize);
    //renderer.transitionImageLayout(image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL, mipLevels, 6);

    vkDestroyBuffer(renderer.m_device, stagingBuffer, nullptr);
    vkFreeMemory(renderer.m_device, stagingBufferMemory, nullptr);

    renderer.GenerateMipmapsCube(m_image, VK_FORMAT_R32G32B32A32_SFLOAT, m_width, m_height, m_mipLevels);
}

void Texture3D::CreateTextureImageView()
{
	m_view = Renderer::GetInstance().CreateImageViewCube(m_image, VK_FORMAT_R32G32B32A32_SFLOAT, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels);
}

void Texture3D::CreateTextureSampler()
{
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

    if (vkCreateSampler(Renderer::GetInstance().m_device, &samplerInfo, nullptr, &m_sampler) != VK_SUCCESS)
    {
        throw std::runtime_error("failed to create texture sampler!");
    }   
}

void Texture3D::Destroy()
{
	Renderer& renderer = Renderer::GetInstance();
    vkDestroyImageView(renderer.m_device, m_view, nullptr);
    vkDestroyImage(renderer.m_device, m_image, nullptr);
    vkFreeMemory(renderer.m_device, m_deviceMemory, nullptr);
    vkDestroySampler(renderer.m_device, m_sampler, nullptr);
}


Primitive::Primitive(uint32_t firstIndex, uint32_t indexCount, uint32_t vertexCount, Material* material) : 
    m_firstIndex(firstIndex),
    m_indexCount(indexCount),
    m_vertexCount(vertexCount),
    m_material(material)
{
    m_hasIndices = indexCount > 0;
}

void Primitive::SetBoundingBox(glm::vec3 const& min, glm::vec3 const& max)
{
    m_boundingBox.m_min = min;
    m_boundingBox.m_max = max;
    m_boundingBox.m_valid = true;
}

Mesh::Mesh(glm::mat4 const& matrix)
{
    Renderer& renderer = Renderer::GetInstance();

    m_uniformBlock.m_matrix = matrix;
    
    renderer.CreateBuffer(
        sizeof(m_uniformBlock),
        VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, 
        m_uniformBuffer.m_buffer,
        m_uniformBuffer.m_memory
    );

    vkMapMemory(renderer.m_device, m_uniformBuffer.m_memory, 0, sizeof(m_uniformBlock), 0, &m_uniformBuffer.m_mapped);
    memcpy(m_uniformBuffer.m_mapped, &m_uniformBlock, sizeof(m_uniformBlock));

    m_uniformBuffer.m_descriptor = { m_uniformBuffer.m_buffer, 0, sizeof(m_uniformBlock) };
};

Mesh::~Mesh()
{
	Renderer& renderer = Renderer::GetInstance();

    vkDestroyBuffer(renderer.m_device, m_uniformBuffer.m_buffer, nullptr);
    vkFreeMemory(renderer.m_device, m_uniformBuffer.m_memory, nullptr);
    
    for (Primitive const* primitive : m_primitives)
    {
		delete primitive;
    }
}

void Mesh::SetBoundingBox(glm::vec3 const& min, glm::vec3 const& max)
{
    m_boundingBox.m_min = min;
    m_boundingBox.m_max = max;
    m_boundingBox.m_valid = true;
}


glm::mat4 Node::GetLocalMatrix()
{
    return glm::translate(glm::mat4(1.0f), m_translation) * glm::mat4(m_rotation) * glm::scale(glm::mat4(1.0f), m_scale) * m_matrix;
}

glm::mat4 Node::GetWorldMatrix()
{
    glm::mat4 matrix = GetLocalMatrix();
    
    Node* parent = m_parent;
    while (parent)
    {
        matrix = parent->GetLocalMatrix() * matrix;
        parent = parent->m_parent;
    }
    
    return matrix;
}

void Node::Update()
{
    if (m_mesh)
    {
        m_mesh->m_uniformBlock.m_matrix = GetWorldMatrix();
        memcpy(m_mesh->m_uniformBuffer.m_mapped, &m_mesh->m_uniformBlock, sizeof(m_mesh->m_uniformBlock));
    }

    for (Node* child : m_children)
    {
        child->Update();
    }
}

Node::~Node()
{
    if (m_mesh)
    {
        delete m_mesh;
    }

    for (Node* child : m_children)
    {
        delete child;
    }
}


void Model::Destroy()
{
	Renderer& renderer = Renderer::GetInstance();

    if (m_vertices.m_buffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(renderer.m_device, m_vertices.m_buffer, nullptr);
        vkFreeMemory(renderer.m_device, m_vertices.m_memory, nullptr);
    }

    if (m_indices.m_buffer != VK_NULL_HANDLE)
    {
        vkDestroyBuffer(renderer.m_device, m_indices.m_buffer, nullptr);
        vkFreeMemory(renderer.m_device, m_indices.m_memory, nullptr);
    }

    for (Texture2D& texture : m_textures)
    {
        texture.Destroy();
    }

    m_textures.resize(0);
    m_textureSamplers.resize(0);

    for (Node* node : m_nodes)
    {
        delete node;
    }

    m_materials.resize(0);
    m_nodes.resize(0);
    m_linearNodes.resize(0);
};

void Model::LoadNode(Node* parent, tinygltf::Node const& node, uint32_t const nodeIndex, tinygltf::Model const& model, std::vector<uint32_t>& indexBuffer, std::vector<Vertex>& vertexBuffer)
{
    Node* newNode = new Node{};
    newNode->m_index = nodeIndex;
    newNode->m_parent = parent;
    newNode->m_matrix = glm::mat4(1.0f);

    // Generate local node matrix
    glm::vec3 translation = glm::vec3(0.0f);
    if (node.translation.size() == 3)
    {
        translation = glm::make_vec3(node.translation.data());
        newNode->m_translation = translation;
    }
    
    if (node.rotation.size() == 4)
    {
        glm::quat q = glm::make_quat(node.rotation.data());
        newNode->m_rotation = glm::mat4(q);
    }
    
    glm::vec3 scale = glm::vec3(1.0f);
    if (node.scale.size() == 3)
    {
        scale = glm::make_vec3(node.scale.data());
        newNode->m_scale = scale;
    }
    
    if (node.matrix.size() == 16)
    {
        newNode->m_matrix = glm::make_mat4x4(node.matrix.data());
    }

    // Node with children
    if (node.children.size() > 0)
    {
        for (size_t i = 0; i < node.children.size(); i++)
        {
            LoadNode(newNode, model.nodes[node.children[i]], node.children[i], model, indexBuffer, vertexBuffer);
        }
    }

    // Node contains mesh data
    if (node.mesh > -1)
    {
        tinygltf::Mesh const& mesh = model.meshes[node.mesh];
        Mesh* newMesh = new Mesh(newNode->m_matrix);
        for (size_t j = 0; j < mesh.primitives.size(); j++)
        {
            tinygltf::Primitive const& primitive = mesh.primitives[j];
            uint32_t indexStart = static_cast<uint32_t>(indexBuffer.size());
            uint32_t vertexStart = static_cast<uint32_t>(vertexBuffer.size());
            uint32_t indexCount = 0;
            uint32_t vertexCount = 0;
            glm::vec3 posMin{};
            glm::vec3 posMax{};
            bool const hasIndices = primitive.indices > -1;
            
            // Vertices
            {
				float const* bufferPos = nullptr;
				float const* bufferNormals = nullptr;
				float const* bufferTexCoordSet0 = nullptr;
				float const* bufferTexCoordSet1 = nullptr;

                // Position attribute is required
                assert(primitive.attributes.find("POSITION") != primitive.attributes.end());

                tinygltf::Accessor const& posAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
                tinygltf::BufferView const& posView = model.bufferViews[posAccessor.bufferView];
                bufferPos = reinterpret_cast<float const*>(&(model.buffers[posView.buffer].data[posAccessor.byteOffset + posView.byteOffset]));
                posMin = glm::vec3(posAccessor.minValues[0], posAccessor.minValues[1], posAccessor.minValues[2]);
                posMax = glm::vec3(posAccessor.maxValues[0], posAccessor.maxValues[1], posAccessor.maxValues[2]);
                vertexCount = static_cast<uint32_t>(posAccessor.count);

                if (primitive.attributes.find("NORMAL") != primitive.attributes.end())
                {
                    tinygltf::Accessor const& normAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
                    tinygltf::BufferView const& normView = model.bufferViews[normAccessor.bufferView];
                    bufferNormals = reinterpret_cast<float const*>(&(model.buffers[normView.buffer].data[normAccessor.byteOffset + normView.byteOffset]));
                }

                if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end())
                {
                    tinygltf::Accessor const& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
                    tinygltf::BufferView const& uvView = model.bufferViews[uvAccessor.bufferView];
                    bufferTexCoordSet0 = reinterpret_cast<float const*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                }

                if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end())
                {
                    tinygltf::Accessor const& uvAccessor = model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
                    tinygltf::BufferView const& uvView = model.bufferViews[uvAccessor.bufferView];
                    bufferTexCoordSet1 = reinterpret_cast<float const*>(&(model.buffers[uvView.buffer].data[uvAccessor.byteOffset + uvView.byteOffset]));
                }

                for (size_t v = 0; v < posAccessor.count; v++)
                {
                    Vertex vertex{};
                    vertex.m_position = glm::vec4(glm::make_vec3(&bufferPos[v * 3]), 1.0f);
                    vertex.m_normal = glm::normalize(glm::vec3(bufferNormals ? glm::make_vec3(&bufferNormals[v * 3]) : glm::vec3(0.0f)));
                    vertex.m_uvSet0 = bufferTexCoordSet0 ? glm::make_vec2(&bufferTexCoordSet0[v * 2]) : glm::vec3(0.0f);
                    vertex.m_uvSet1 = bufferTexCoordSet1 ? glm::make_vec2(&bufferTexCoordSet1[v * 2]) : glm::vec3(0.0f);

                    vertexBuffer.push_back(vertex);
                }
            }

            // Indices
            if (hasIndices)
            {
                tinygltf::Accessor const& accessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
                tinygltf::BufferView const& bufferView = model.bufferViews[accessor.bufferView];
                tinygltf::Buffer const& buffer = model.buffers[bufferView.buffer];

                indexCount = static_cast<uint32_t>(accessor.count);
                void const* const dataPtr = &(buffer.data[accessor.byteOffset + bufferView.byteOffset]);

                switch (accessor.componentType)
                {
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
                    uint32_t const* buf = static_cast<uint32_t const*>(dataPtr);
                    for (size_t index = 0; index < accessor.count; index++)
                    {
                        indexBuffer.push_back(buf[index] + vertexStart);
                    }
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
                    uint16_t const* buf = static_cast<uint16_t const*>(dataPtr);
                    for (size_t index = 0; index < accessor.count; index++)
                    {
                        indexBuffer.push_back(buf[index] + vertexStart);
                    }
                    break;
                }
                case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
                    uint8_t const* buf = static_cast<uint8_t const*>(dataPtr);
                    for (size_t index = 0; index < accessor.count; index++)
                    {
                        indexBuffer.push_back(buf[index] + vertexStart);
                    }
                    break;
                }
                default:
                    std::cerr << "Index component type " << accessor.componentType << " not supported!" << std::endl;
                    return;
                }
            }

            Primitive* newPrimitive = new Primitive(indexStart, indexCount, vertexCount, primitive.material > -1 ? &m_materials[primitive.material] : &m_materials.back());
            newPrimitive->SetBoundingBox(posMin, posMax);
            newMesh->m_primitives.push_back(newPrimitive);
        }

        // Mesh BB from BBs of primitives
        for (Primitive const* primitive : newMesh->m_primitives)
        {
            if (primitive->m_boundingBox.m_valid && !newMesh->m_boundingBox.m_valid)
            {
                newMesh->m_boundingBox = primitive->m_boundingBox;
                newMesh->m_boundingBox.m_valid = true;
            }
            newMesh->m_boundingBox.m_min = glm::min(newMesh->m_boundingBox.m_min, primitive->m_boundingBox.m_min);
            newMesh->m_boundingBox.m_max = glm::max(newMesh->m_boundingBox.m_max, primitive->m_boundingBox.m_max);
        }
        newNode->m_mesh = newMesh;
    }

    if (parent)
    {
        parent->m_children.push_back(newNode);
    } 
    else
    {
        m_nodes.push_back(newNode);
    }
    m_linearNodes.push_back(newNode);
}

void Model::LoadTextures(tinygltf::Model const& gltfModel)
{
    for (tinygltf::Texture const& gltfTexture : gltfModel.textures)
    {
        tinygltf::Image const& image = gltfModel.images[gltfTexture.source];
        
        TextureSampler textureSampler{};
        if (gltfTexture.sampler != -1)
        {
            textureSampler = m_textureSamplers[gltfTexture.sampler];
        }

        Texture2D texture;
        texture.FromglTfImage(image, textureSampler);
        m_textures.push_back(texture);
    }
}

VkSamplerAddressMode Model::GetVkWrapMode(int32_t const wrapMode) 
{
    switch (wrapMode)
    {
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

VkFilter Model::GetVkFilterMode(int32_t const filterMode) 
{
    switch (filterMode)
    {
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
    for (tinygltf::Sampler const& gltfSampler : gltfModel.samplers)
    {
        TextureSampler sampler{};
        sampler.m_minFilter = GetVkFilterMode(gltfSampler.minFilter);
        sampler.m_magFilter = GetVkFilterMode(gltfSampler.magFilter);
        sampler.m_addressModeU = GetVkWrapMode(gltfSampler.wrapS);
        sampler.m_addressModeV = GetVkWrapMode(gltfSampler.wrapT);
        sampler.m_addressModeW = sampler.m_addressModeV;
        m_textureSamplers.push_back(sampler);
    }
}

void Model::LoadMaterials(tinygltf::Model& gltfModel)
{
    for (tinygltf::Material& gltfMaterial : gltfModel.materials)
    {
        Material material{};
        if (gltfMaterial.values.find("baseColorTexture") != gltfMaterial.values.end())
        {
            material.m_baseColorTexture = &m_textures[gltfMaterial.values["baseColorTexture"].TextureIndex()];
            material.m_baseColorTextCoordSet = gltfMaterial.values["baseColorTexture"].TextureTexCoord();
        }
        if (gltfMaterial.values.find("metallicRoughnessTexture") != gltfMaterial.values.end())
        {
            material.m_metallicRoughnessTexture = &m_textures[gltfMaterial.values["metallicRoughnessTexture"].TextureIndex()];
            material.m_metallicRoughnessTextCoordSet = gltfMaterial.values["metallicRoughnessTexture"].TextureTexCoord();
        }
        if (gltfMaterial.values.find("roughnessFactor") != gltfMaterial.values.end())
        {
            material.m_roughnessFactor = static_cast<float>(gltfMaterial.values["roughnessFactor"].Factor());
        }
        if (gltfMaterial.values.find("metallicFactor") != gltfMaterial.values.end())
        {
            material.m_metallicFactor = static_cast<float>(gltfMaterial.values["metallicFactor"].Factor());
        }
        if (gltfMaterial.values.find("baseColorFactor") != gltfMaterial.values.end())
        {
            material.m_baseColorFactor = glm::make_vec4(gltfMaterial.values["baseColorFactor"].ColorFactor().data());
        }				
        if (gltfMaterial.additionalValues.find("normalTexture") != gltfMaterial.additionalValues.end())
        {
            material.m_normalTexture = &m_textures[gltfMaterial.additionalValues["normalTexture"].TextureIndex()];
            material.m_normalTextCoordSet = gltfMaterial.additionalValues["normalTexture"].TextureTexCoord();
        }
        if (gltfMaterial.additionalValues.find("emissiveTexture") != gltfMaterial.additionalValues.end())
        {
            material.m_emissiveTexture = &m_textures[gltfMaterial.additionalValues["emissiveTexture"].TextureIndex()];
            material.m_emissiveTextCoordSet = gltfMaterial.additionalValues["emissiveTexture"].TextureTexCoord();
        }
        if (gltfMaterial.additionalValues.find("occlusionTexture") != gltfMaterial.additionalValues.end())
        {
            material.m_occlusionTexture = &m_textures[gltfMaterial.additionalValues["occlusionTexture"].TextureIndex()];
            material.m_occlusionTextCoordSet = gltfMaterial.additionalValues["occlusionTexture"].TextureTexCoord();
        }
        if (gltfMaterial.additionalValues.find("alphaMode") != gltfMaterial.additionalValues.end())
        {
            tinygltf::Parameter param = gltfMaterial.additionalValues["alphaMode"];
            if (param.string_value == "BLEND")
            {
                material.m_alphaMode = Material::ALPHAMODE_BLEND;
            }
            if (param.string_value == "MASK")
            {
                material.m_alphaCutoff = 0.5f;
                material.m_alphaMode = Material::ALPHAMODE_MASK;
            }
        }
        if (gltfMaterial.additionalValues.find("alphaCutoff") != gltfMaterial.additionalValues.end())
        {
            material.m_alphaCutoff = static_cast<float>(gltfMaterial.additionalValues["alphaCutoff"].Factor());
        }
        if (gltfMaterial.additionalValues.find("emissiveFactor") != gltfMaterial.additionalValues.end())
        {
            material.m_emissiveFactor = glm::vec4(glm::make_vec3(gltfMaterial.additionalValues["emissiveFactor"].ColorFactor().data()), 1.0);
            material.m_emissiveFactor = glm::vec4(0.0f);
        }

        m_materials.push_back(material);
    }

    // Push a default material at the end of the list for meshes with no material assigned
    m_materials.push_back(Material());
}

void Model::LoadFromFile(std::string const filename)
{
    tinygltf::Model gltfModel;
    tinygltf::TinyGLTF gltfContext;
    std::string error;
    std::string warning;

    bool binary = false;
    size_t extpos = filename.rfind('.', filename.length());
    if (extpos != std::string::npos)
    {
        binary = (filename.substr(extpos + 1, filename.length() - extpos) == "glb");
    }

    bool fileLoaded = binary ? 
        gltfContext.LoadBinaryFromFile(&gltfModel, &error, &warning, filename.c_str()) :
        gltfContext.LoadASCIIFromFile(&gltfModel, &error, &warning, filename.c_str());

    std::vector<Vertex> vertexBuffer;
    std::vector<uint32_t> indexBuffer;

    if (fileLoaded)
    {
        LoadTextureSamplers(gltfModel);
        LoadTextures(gltfModel);
        LoadMaterials(gltfModel);

        tinygltf::Scene const& scene = gltfModel.scenes[gltfModel.defaultScene > -1 ? gltfModel.defaultScene : 0];
        for (size_t i = 0; i < scene.nodes.size(); i++)
        {
            tinygltf::Node const& node = gltfModel.nodes[scene.nodes[i]];
            LoadNode(nullptr, node, scene.nodes[i], gltfModel, indexBuffer, vertexBuffer);
        }

        for (Node* node : m_linearNodes)
        {
            // Initial pose
            if (node->m_mesh)
            {
                node->Update();
            }
        }
    }
    else
    {
        std::cerr << "Could not load gltf file: " << error << std::endl;
        return;
    }

    m_indices.m_count = static_cast<uint32_t>(indexBuffer.size());

    CreateVertexBuffer(vertexBuffer);
    if (m_indices.m_count > 0)
    { 
        CreateIndexBuffer(indexBuffer);
    }

    GetSceneDimensions();
}

void Model::DrawNode(Node const* node, VkCommandBuffer commandBuffer)
{
    if (node->m_mesh)
    {
        for (Primitive const* primitive : node->m_mesh->m_primitives)
        {
            vkCmdDrawIndexed(commandBuffer, primitive->m_indexCount, 1, primitive->m_firstIndex, 0, 0);
        }
    }

    for (Node const* child : node->m_children)
    {
        DrawNode(child, commandBuffer);
    }
}

void Model::Draw(VkCommandBuffer commandBuffer)
{
    VkDeviceSize const offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &m_vertices.m_buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, m_indices.m_buffer, 0, VK_INDEX_TYPE_UINT32);
    
    for (Node const* node : m_nodes)
    {
        DrawNode(node, commandBuffer);
    }
}

void Model::CalculateBoundingBox(Node* node, Node const* parent)
{
    if (node->m_mesh)
    {
        if (node->m_mesh->m_boundingBox.m_valid)
        {
            node->m_boundingBox2 = node->m_mesh->m_boundingBox.FromMatrix(node->GetWorldMatrix());
            if (node->m_children.size() == 0)
            {
                node->m_boundingBox.m_min = node->m_boundingBox2.m_min;
                node->m_boundingBox.m_max = node->m_boundingBox2.m_max;
                node->m_boundingBox.m_valid = true;
            }
        }
    }

    for (Node* child : node->m_children)
    {
        CalculateBoundingBox(child, node);
    }
}

void Model::GetSceneDimensions()
{
    // Calculate binary volume hierarchy for all nodes in the scene
    for (Node* node : m_linearNodes)
    {
        CalculateBoundingBox(node, nullptr);
    }

    m_dimensions.m_min = glm::vec3(FLT_MAX);
    m_dimensions.m_max = glm::vec3(-FLT_MAX);

    for (Node const* node : m_linearNodes)
    {
        if (node->m_boundingBox.m_valid)
        {
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

void Model::CreateVertexBuffer(std::vector<Vertex> const& vertices)
{
	Renderer& renderer = Renderer::GetInstance();

    VkDeviceSize bufferSize = sizeof(vertices[0]) * vertices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer.m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
    vkUnmapMemory(renderer.m_device, stagingBufferMemory);

    renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertices.m_buffer, m_vertices.m_memory);

    renderer.CopyBuffer(stagingBuffer, m_vertices.m_buffer, bufferSize);

    vkDestroyBuffer(renderer.m_device, stagingBuffer, nullptr);
    vkFreeMemory(renderer.m_device, stagingBufferMemory, nullptr);
}

void Model::CreateIndexBuffer(std::vector<uint32_t> const& indices)
{
	Renderer& renderer = Renderer::GetInstance();

    VkDeviceSize bufferSize = sizeof(indices[0]) * indices.size();

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingBufferMemory;
    renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

    void* data;
    vkMapMemory(renderer.m_device, stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), (size_t) bufferSize);
    vkUnmapMemory(renderer.m_device, stagingBufferMemory);

    renderer.CreateBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indices.m_buffer, m_indices.m_memory);

    renderer.CopyBuffer(stagingBuffer, m_indices.m_buffer, bufferSize);

    vkDestroyBuffer(renderer.m_device, stagingBuffer, nullptr);
    vkFreeMemory(renderer.m_device, stagingBufferMemory, nullptr);
}

VkVertexInputBindingDescription Model::Vertex::GetBindingDescription()
{
    VkVertexInputBindingDescription bindingDescription = {};
    bindingDescription.binding = 0;
    bindingDescription.stride = sizeof(Vertex);
    bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

    return bindingDescription;
}

std::array<VkVertexInputAttributeDescription, 4> Model::Vertex::GetAttributeDescriptions()
{
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