#pragma once

#include <Resources/ImageResource.hpp>
#include <Utilities/Helpers.hpp>

struct AttachmentCreationInfo
{
    ImageCreateInfo m_imageCreateInfo;
};

class AttachmentResource
    : public std::enable_shared_from_this<AttachmentResource>
{
public:
    AttachmentResource();
    ~AttachmentResource();

    AttachmentResource(AttachmentResource&&) noexcept;
    AttachmentResource& operator=(AttachmentResource&&) noexcept;
    AttachmentResource(AttachmentResource const&) = delete;
    AttachmentResource& operator=(AttachmentResource const&) = delete;

    inline SharedPtr<AttachmentResource> GetSharedPtr() { return shared_from_this(); }

    void Create();
    void Destroy();

    inline AttachmentCreationInfo const& GetCreationInfo() const { return m_creationInfo; }
    inline ImageCreateInfo const& GetImageCreationInfo() const { return m_creationInfo.m_imageCreateInfo; }
    inline void SetReadInPass(uint64_t passId) { m_readInPasses.insert(passId); }
    inline void SetWrittenInPass(uint64_t passId) { m_writtenInPasses.insert(passId); }
    inline ImageResource const& GetImage() const { return *m_image; }
    inline ImageResource& GetImage() { return *m_image; }
    inline SharedPtr<ImageResource>& GetImagePtr() { return m_image; }
    inline bool IsUsed() { return !m_writtenInPasses.empty() || !m_readInPasses.empty(); }
    inline bool IsPersistent() { return m_isPersistent; }
    void SetCreationInfo(AttachmentCreationInfo const& creationInfo);
    inline void SetIsPersistent(bool persistent) { m_isPersistent = persistent; }
    VkClearValue GetClearValue() const;
    bool IsReadInPass(uint64_t passId) const;
    bool IsWrittenInPass(uint64_t passId) const;
    void RemovePassUsage(uint64_t passId);
    bool IsValid() const;

private:
    SharedPtr<ImageResource> m_image = nullptr;

    AttachmentCreationInfo m_creationInfo;
    std::set<uint64_t> m_readInPasses;
    std::set<uint64_t> m_writtenInPasses;
    std::optional<VkClearValue> m_clearValue;
    bool m_isPersistent = false;
};
