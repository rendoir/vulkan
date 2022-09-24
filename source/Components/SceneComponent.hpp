#pragma once

#include <Components/EntityComponent.hpp>
#include <Resources/Buffer.hpp>
#include <Resources/Descriptor.hpp>
#include <Resources/ResourceComponent.hpp>
#include <Resources/ResourceHandle.hpp>
#include <Systems/ResourceSystem.hpp>
#include <Utilities/Singleton.hpp>

class SceneComponent : public EntityComponent
{
public:
    SceneComponent() = default;
    virtual ~SceneComponent();

    entt::entity GetParent() const { return m_parent; }
    std::vector<entt::entity> const& GetChildren() const { return m_children; }

    bool AttachTo(entt::entity parent);
    bool Detach();

    // Local transform
    inline glm::vec3 const& GetLocalTranslation() const { return m_localTransform.GetTranslation(); }
    inline glm::quat const& GetLocalRotation() const { return m_localTransform.GetRotation(); }
    inline glm::vec3 const& GetLocalScale() const { return m_localTransform.GetScale(); }
    inline glm::vec3 GetLocalEulerRotation() const { return m_localTransform.GetEulerRotation(); }
    inline glm::mat4 const& GetLocalMatrix() { return m_localTransform.GetMatrix(); }
    void SetLocalTranslation(glm::vec3 const& translation);
    void SetLocalScale(glm::vec3 const& scale);
    void SetLocalRotation(glm::quat const& rotation);
    void SetLocalEulerRotation(glm::vec3 const& eulerRotation);

    // World transform
    glm::vec3 const& GetWorldTranslation();
    glm::quat const& GetWorldRotation();
    glm::vec3 const& GetWorldScale();
    glm::vec3 GetWorldEulerRotation();
    glm::mat4 const& GetWorldMatrix();
    void SetWorldTranslation(glm::vec3 const& translation);
    void SetWorldScale(glm::vec3 const& scale);
    void SetWorldRotation(glm::quat const& rotation);
    void SetWorldEulerRotation(glm::vec3 const& eulerRotation);

private:
    struct Transform
    {
        Transform() = default;
        Transform(glm::vec3 const& translation, glm::quat const& rotation, glm::vec3 const& scale);

        // Getters
        inline glm::vec3 const& GetTranslation() const { return m_translation; }
        inline glm::quat const& GetRotation() const { return m_rotation; }
        inline glm::vec3 const& GetScale() const { return m_scale; }
        glm::vec3 GetEulerRotation() const;
		glm::mat4 const& GetMatrix();

        // Setters
        void SetTranslation(glm::vec3 const& translation);
		void SetScale(glm::vec3 const& scale);
		void SetRotation(glm::quat const& rotation);
		void SetEulerRotation(glm::vec3 const& eulerRotation);

        // Modifiers
        void Translate(glm::vec3 const& translation);
		void Scale(glm::vec3 const& scale);
		void Rotate(glm::vec3 const& eulerRotation);
		void Rotate(glm::quat const& rotation);

    private:
        void CalculateMatrix();

    private:
        glm::vec3 m_translation{ 0.0f };
        glm::quat m_rotation{ 0.0f, 0.0f, 0.0f, 1.0f };
        glm::vec3 m_scale{ 1.0f };

        glm::mat4 m_matrix;
        bool m_isMatrixDirty = true;
    };

private:
    void InvalidateWorldTransformRecursively();
    void CalculateWorldTransform();
    bool IsDescendent(entt::entity const entity) const;

private:
    Transform m_localTransform;
    Transform m_worldTransform;
    bool m_isWorldTransformDirty = true;

    entt::entity m_parent = entt::null;
    std::vector<entt::entity> m_children;
};

class SceneComponentResource : public ComponentResourceInFlight
{
public:
    struct UniformData 
    {
        glm::mat4 m_worldMatrix;
    };

public:
    SceneComponentResource(SceneComponent const& scene);
    
public:
    ResourceHandleInFlight<Buffer> m_uniformBuffer;

    static const std::vector<VkDescriptorSetLayoutBinding> ms_bindings;
};

class SceneResourceSystem : public ResourceSystem<SceneComponent, SceneComponentResource>, public Singleton<SceneResourceSystem>
{
public:
    void Update() override;
};
