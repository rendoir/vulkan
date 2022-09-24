#include <Resources/Buffer.hpp>

#include <Systems/Renderer.hpp>
#include <Utilities/Helpers.hpp>

/// Buffer
Buffer::Buffer(BufferInfo const& info)
{
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = info.m_size;
    bufferInfo.usage = info.m_usage;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VmaAllocationCreateInfo allocationInfo = {};
    allocationInfo.usage = info.m_memoryUsage;

    if (vmaCreateBuffer(Renderer::GetInstance().GetAllocator(), &bufferInfo, &allocationInfo, &m_buffer, &m_allocation, nullptr) != VK_SUCCESS)
    {
        ThrowError("Failed to create buffer.");
    }
}

Buffer::Buffer(Buffer&& other) noexcept
{
    m_buffer = other.m_buffer;
    m_allocation = other.m_allocation;

    other.m_buffer = VK_NULL_HANDLE;
    other.m_allocation = VK_NULL_HANDLE;
}

Buffer& Buffer::operator=(Buffer&& other) noexcept
{
    if (this == &other)
    {
        return *this;
    }

    Destroy();

    m_buffer = other.m_buffer;
    m_allocation = other.m_allocation;

    other.m_buffer = VK_NULL_HANDLE;
    other.m_allocation = VK_NULL_HANDLE;

    return *this;
}

Buffer::~Buffer()
{
    Destroy();    
}

void Buffer::Destroy()
{
    if (m_buffer != VK_NULL_HANDLE &&
        m_allocation != VK_NULL_HANDLE)
    {
        vmaDestroyBuffer(Renderer::GetInstance().GetAllocator(), m_buffer, m_allocation);
    }

    m_buffer = VK_NULL_HANDLE;
    m_allocation = VK_NULL_HANDLE;
}

void* Buffer::MapMemory()
{
    void* mappedMemory;
    vmaMapMemory(Renderer::GetInstance().GetAllocator(), m_allocation, &mappedMemory);
    return mappedMemory;
}

void Buffer::UnmapMemory()
{
    vmaUnmapMemory(Renderer::GetInstance().GetAllocator(), m_allocation); 
}

void Buffer::CopyDataToBuffer(void const* const data, VkDeviceSize size)
{
    void* mappedMemory = MapMemory();
    memcpy(mappedMemory, data, size);
    UnmapMemory();
}

void Buffer::CopyDataFromBuffer(VkCommandBuffer commandBuffer, Buffer const& srcBuffer, VkDeviceSize size)
{
    VkBufferCopy copyRegion = {};
    copyRegion.size = size;
    
    vkCmdCopyBuffer(commandBuffer, srcBuffer.m_buffer, m_buffer, 1, &copyRegion);
}

/// SelfUpdatableBuffer
SelfUpdatableBuffer::SelfUpdatableBuffer(SelfUpdatableBufferInfo const& info)
    : Buffer(info)
{
    m_updateCallback = info.m_updateCallback;
}

SelfUpdatableBuffer::SelfUpdatableBuffer(SelfUpdatableBuffer&& other) noexcept
    : Buffer(std::move(other))
{
    m_isDirty = other.m_isDirty;
    m_updateCallback = std::move(other.m_updateCallback);
}

SelfUpdatableBuffer& SelfUpdatableBuffer::operator=(SelfUpdatableBuffer&& other) noexcept
{
    Buffer::operator=(std::move(other));

    if (this == &other)
    {
        return *this;
    }

    m_isDirty = other.m_isDirty;
    m_updateCallback = std::move(other.m_updateCallback);

    return *this;
}

void SelfUpdatableBuffer::Update()
{
    if (IsDirty() && m_updateCallback)
    {
        Renderer& renderer = Renderer::GetInstance();

        void* mappedData;
        vmaMapMemory(renderer.GetAllocator(), m_allocation, &mappedData);
            m_updateCallback(mappedData);
        vmaUnmapMemory(renderer.GetAllocator(), m_allocation);

        m_isDirty = false;
    }
}
