#include "pch.h"
#include "ECSFace.h"
#include "Debug/Intrumentator.h"
namespace Game
{
    ECSImplementation ECSFace::s_ECSImplementation = ECSImplementation::InternalEcs;
    void* ECSFace::m_Registry = nullptr;

    uint64_t ECSFace::CreateEntity()
    {
        GAME_PROFILE_FUNCTION();
        switch(s_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: 
            {
                ecs::Scene* scene = ECSFace::CastTo<ecs::Scene>();
                return scene->CreateEntity();
            }
            case ECSImplementation::Entt: 
                entt::registry* scene = ECSFace::CastTo<entt::registry>();
                return (uint32_t)scene->create();
            break;
        }
    }

    void ECSFace::DestroyEntity(uint64_t ent)
    {
        GAME_PROFILE_FUNCTION();
        switch(s_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: 
            {
                ecs::Scene* scene = ECSFace::CastTo<ecs::Scene>();
                scene->DestroyEntity(ent);
            }
            case ECSImplementation::Entt: 
                entt::registry* scene = ECSFace::CastTo<entt::registry>();
                scene->destroy((entt::entity)ent);
            break;
        }
    }
}