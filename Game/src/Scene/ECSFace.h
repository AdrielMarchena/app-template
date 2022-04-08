#pragma once
 
// #include "Scene/Entity.h"
#include "entt/entt.hpp"
#include "Ecs/ECSScene.h"
#include "Ecs/SceneView.h"

#include <string>

namespace Game
{

enum class ECSImplementation
{
    Entt,
    InternalEcs
};
class ECSFace
{
private:
    static void* m_Registry;
    static ECSImplementation s_ECSImplementation;
public:
    static void SetImplementation(ECSImplementation implementation);

    static void CreateRegistry()
    {
        if(s_ECSImplementation == ECSImplementation::Entt)
        {
            // TODO: Implement
        }
        if(s_ECSImplementation == ECSImplementation::InternalEcs)
        {
            m_Registry = new ecs::Scene();
        }
    }

    //Ecs stuff
    static uint64_t CreateEntity(); // In Cpp file
    static void DestroyEntity(uint64_t ent); // In Cpp file

    static ECSImplementation GetECSImplementation() { return s_ECSImplementation; }
    static std::string GetECSImplementationAsString()
    {
        if(s_ECSImplementation == ECSImplementation::Entt) return "Entt";
        if(s_ECSImplementation == ECSImplementation::InternalEcs) return "InternalEcs";
    }

    template<typename T>
    inline static T& GetComponent(uint64_t entity)
    {
        switch(s_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: return ECSGetComponent<T>((ecs::entity)entity);
            case ECSImplementation::Entt: uint32_t en = entity; return EnttGetComponent<T>(entt::entity{en});
        }
    }

    template<typename T, class... _Args>
    inline static T& AddComponent(ecs::entity entity, _Args&&... args)
    {
        switch(s_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: return ECSAddComponent<T>((ecs::entity)entity, std::forward<_Args>(args)...);
            case ECSImplementation::Entt: uint32_t en = entity; return EnttAddComponent<T>(entt::entity{en});
        }
    }

    template<typename T>
    inline static bool ContainComponent(ecs::entity entity)
    {
        switch(s_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: return ECSContainComponent<T>((ecs::entity)entity);
            case ECSImplementation::Entt: uint32_t en = entity; return EnttContainComponent<T>(entt::entity{en});
        }
    }

    template<typename T>
    inline static bool RemoveComponent(ecs::entity entity)
    {
        switch(s_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: return ECSRemoveComponent<T>((ecs::entity)entity);
            case ECSImplementation::Entt: uint32_t en = entity; return EnttRemoveComponent<T>(entt::entity{en});
        }
    }

    template<typename T>
    static std::vector<uint64_t> View()
    {
        std::vector<uint64_t> retView;
        switch(s_ECSImplementation)
        {
            case ECSImplementation::InternalEcs:
            {
                auto ecsV = ecs::SceneView<T>(*static_cast<ecs::Scene*>(ECSFace::CastTo<ecs::Scene>()));
                for(auto vv : ecsV) retView.emplace_back(vv);
                break;
            }
            case ECSImplementation::Entt:
            {
                auto enttV = ECSFace::CastTo<entt::registry>()->view<T>();
                for(auto vv : enttV) retView.emplace_back((uint32_t)vv);
                break;
            }
                
        }
        return retView;
    }

private:

    // GET
    template<typename T>
    inline static T& ECSGetComponent(ecs::entity entity)
    {
        ecs::Scene* scene = ECSFace::CastTo<ecs::Scene>();
        return scene->Get<T>(entity);
    }
    template<typename T>
    inline static T& EnttGetComponent(entt::entity entity)
    {
        // TODO: Implement
    }

    // ADD
    template<typename T, class... _Args>
    inline static T& ECSAddComponent(ecs::entity entity, _Args&&... args)
    {
        ecs::Scene* scene = ECSFace::CastTo<ecs::Scene>();
        return scene->Add<T>(entity, std::forward<_Args>(args)...);
    }
    template<typename T>
    inline static T& EnttAddComponent(entt::entity entity)
    {
        // TODO: Implement
    }

    // CONTAIN
    template<typename T>
    inline static bool ECSContainComponent(ecs::entity entity)
    {
        ecs::Scene* scene = ECSFace::CastTo<ecs::Scene>();
        return scene->Contain<T>(entity);
    }
    template<typename T>
    inline static bool EnttContainComponent(entt::entity entity)
    {
        // TODO: Implement
    }

    // REMOVE
    template<typename T>
    inline static void ECSRemoveComponent(ecs::entity entity)
    {
        ecs::Scene* scene = ECSFace::CastTo<ecs::Scene>();
        return scene->Remove<T>(entity);
    }
    template<typename T>
    inline static void EnttRemoveComponent(entt::entity entity)
    {
        // TODO: Implement
    }

    template<typename _T>
    static _T* CastTo()
    {
        GAME_CORE_ASSERT(m_Registry, "Scene registry is invalid");
        return reinterpret_cast<_T*>(m_Registry);
    }
};

}