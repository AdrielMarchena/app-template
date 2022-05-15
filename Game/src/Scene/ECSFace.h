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
    void* m_Registry = nullptr;
    ECSImplementation m_ECSImplementation = ECSImplementation::Entt;
public:
    void SetImplementation(ECSImplementation implementation)
    {
        m_ECSImplementation = implementation;
    }

    void CreateRegistry()
    {
        if(m_ECSImplementation == ECSImplementation::Entt)
        {
            if (m_Registry)
                delete m_Registry;
            m_Registry = new entt::registry();
        }
        if(m_ECSImplementation == ECSImplementation::InternalEcs)
        {
            if (m_Registry)
                delete m_Registry;
            m_Registry = new ecs::Scene();
        }
    }

    ~ECSFace()
    {
        if (m_Registry)
            delete m_Registry;
    }

    //Ecs stuff
    uint64_t CreateEntity() const; // In Cpp file
    void DestroyEntity(uint64_t ent) const; // In Cpp file

    ECSImplementation GetECSImplementation() const { return m_ECSImplementation; }
    std::string GetECSImplementationAsString() const
    {
        if(m_ECSImplementation == ECSImplementation::Entt) return "Entt";
        if(m_ECSImplementation == ECSImplementation::InternalEcs) return "InternalEcs";
    }

    uint32_t GetEntityNumber(uint64_t entity) const
    {
        switch(m_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: return ecs::GetEntityIndex(entity);
            case ECSImplementation::Entt: return entity;
        }
    }

    template<typename T>
    inline T& GetComponent(uint64_t entity) const
    {
        switch(m_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: return ECSGetComponent<T>((ecs::entity)entity);
            case ECSImplementation::Entt: uint32_t en = entity; return EnttGetComponent<T>(entt::entity{en});
        }
    }

    template<typename T, class... _Args>
    inline T& AddComponent(ecs::entity entity, _Args&&... args) const
    {
        switch(m_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: return ECSAddComponent<T>((ecs::entity)entity, std::forward<_Args>(args)...);
            case ECSImplementation::Entt: uint32_t en = entity; return EnttAddComponent<T>(entt::entity{en}, std::forward<_Args>(args)...);
        }
    }

    template<typename T>
    inline bool ContainComponent(ecs::entity entity) const
    {
        switch(m_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: return ECSContainComponent<T>((ecs::entity)entity);
            case ECSImplementation::Entt: uint32_t en = entity; return EnttContainComponent<T>(entt::entity{en});
        }
    }

    template<typename T>
    inline bool RemoveComponent(ecs::entity entity) const
    {
        switch(m_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: return ECSRemoveComponent<T>((ecs::entity)entity);
            case ECSImplementation::Entt: uint32_t en = entity; return EnttRemoveComponent<T>(entt::entity{en});
        }
    }

    template<typename T>
    std::vector<uint64_t> View() const
    {
        std::vector<uint64_t> retView;
        switch(m_ECSImplementation)
        {
            case ECSImplementation::InternalEcs:
            {
                auto ecsV = ecs::SceneView<T>(*static_cast<ecs::Scene*>(CastTo<ecs::Scene>()));
                for(auto vv : ecsV) retView.emplace_back(vv);
                break;
            }
            case ECSImplementation::Entt:
            {
                auto enttV = CastTo<entt::registry>()->view<T>();
                for(auto vv : enttV) retView.emplace_back((uint32_t)vv);
                break;
            }
                
        }
        return retView;
    }

private:

    // GET
    template<typename T>
    inline T& ECSGetComponent(ecs::entity entity) const
    {
        ecs::Scene* scene =CastTo<ecs::Scene>();
        return scene->Get<T>(entity);
    }
    template<typename T>
    inline T& EnttGetComponent(entt::entity entity) const
    {
        entt::registry* scene = CastTo<entt::registry>();
        return scene->get<T>(entity);
    }

    // ADD
    template<typename T, class... _Args>
    inline T& ECSAddComponent(ecs::entity entity, _Args&&... args) const
    {
        ecs::Scene* scene = CastTo<ecs::Scene>();
        return scene->Add<T>(entity, std::forward<_Args>(args)...);
    }
    template<typename T, class... _Args>
    inline T& EnttAddComponent(entt::entity entity, _Args&&... args) const
    {
        entt::registry* scene = CastTo<entt::registry>();
        return scene->emplace<T>(entity, std::forward<_Args>(args)...);
    }

    // CONTAIN
    template<typename T>
    inline bool ECSContainComponent(ecs::entity entity) const
    {
        ecs::Scene* scene = CastTo<ecs::Scene>();
        return scene->Contain<T>(entity);
    }
    template<typename T>
    inline bool EnttContainComponent(entt::entity entity) const
    {
        entt::registry* scene = CastTo<entt::registry>();
        return scene->any_of<T>(entity);
    }

    // REMOVE
    template<typename T>
    inline void ECSRemoveComponent(ecs::entity entity) const
    {
        ecs::Scene* scene = CastTo<ecs::Scene>();
        return scene->Remove<T>(entity);
    }
    template<typename T>
    inline void EnttRemoveComponent(entt::entity entity) const
    {
        entt::registry* scene = CastTo<entt::registry>();
        return scene->remove<T>(entity);
    }

    template<typename _T>
    _T* CastTo() const
    {
        GAME_CORE_ASSERT(m_Registry, "Scene registry is invalid");
        return reinterpret_cast<_T*>(m_Registry);
    }
};

}