#pragma once

struct BufferInfo
{
    uint64_t m_size;
    VkBufferUsageFlags m_usage;
    VmaMemoryUsage m_memoryUsage;
};

class Buffer
{
public:
    Buffer() = default;
    Buffer(BufferInfo const& info);
    Buffer(Buffer const& other) = delete;
    Buffer& operator=(Buffer const& other) = delete;
    Buffer(Buffer&& other) noexcept;
    Buffer& operator=(Buffer&& other) noexcept;
    ~Buffer();

    VkBuffer GetBuffer() const { return m_buffer; }
    void CopyDataToBuffer(void const* const data, VkDeviceSize size);
    void CopyDataFromBuffer(VkCommandBuffer commandBuffer, Buffer const& srcBuffer, VkDeviceSize size);

    void Destroy();

    void* MapMemory();
    void UnmapMemory();

protected:
    VkBuffer m_buffer = VK_NULL_HANDLE;
    VmaAllocation m_allocation = VK_NULL_HANDLE;
};


struct SelfUpdatableBufferInfo : public BufferInfo
{
    using UpdateCallbackType = void(void*);
    std::function<UpdateCallbackType> m_updateCallback = nullptr;
};

class SelfUpdatableBuffer : public Buffer
{
public:
    using UpdateCallbackType = void(void*);

public:
    SelfUpdatableBuffer(SelfUpdatableBufferInfo const& info);
    SelfUpdatableBuffer(SelfUpdatableBuffer&& other) noexcept;
    SelfUpdatableBuffer& operator=(SelfUpdatableBuffer&& other) noexcept;

    inline void MarkAsDirty() { m_isDirty = true; }
    inline bool IsDirty() { return m_isDirty; }
    void Update();

private:
    bool m_isDirty = true;
    std::function<UpdateCallbackType> m_updateCallback = nullptr;
};
