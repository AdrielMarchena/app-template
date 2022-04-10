#include "pch.h"
#include "Components.h"

#include "entt/entt.hpp"

namespace Game
{
    static inline void StandardReflectionTypes()
    {
        static std::hash<std::string_view> hash{};
        auto factory = entt::meta<std::string>().type(hash("std::string"));
    }

    void InitMetaReflectionComponents()
    {
        StandardReflectionTypes();
        std::hash<std::string_view> hash{};

        { // TagComponent
            auto factory = entt::meta<TagComponent>().type(hash("TagComponent"));
            factory.data<&TagComponent::Tag>(hash("Tag"));
        }

        { // SpriteComponent
            auto factory = entt::meta<SpriteComponent>().type(hash("SpriteComponent"));
            factory
                .data<&SpriteComponent::Color>(hash("Color"))
                .data<&SpriteComponent::Texture>(hash("Texture"))
                .data<&SpriteComponent::Visible>(hash("Visible"));
        }

        { // TransformComponent
            auto factory = entt::meta<TransformComponent>().type(hash("TransformComponent"));
            factory
                .data<&TransformComponent::Translation>(hash("Translation"))
                .data<&TransformComponent::Rotation>(hash("Rotation"))
                .data<&TransformComponent::Scale>(hash("Scale"));
        }
    }
}