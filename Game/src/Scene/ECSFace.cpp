#include "pch.h"
#include "ECSFace.h"

namespace Game
{
    ECSImplementation ECSFace::s_ECSImplementation = ECSImplementation::InternalEcs;
    void* ECSFace::m_Registry = nullptr;

    uint64_t ECSFace::CreateEntity()
    {
        switch(s_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: 
            {
                ecs::Scene* scene = ECSFace::CastTo<ecs::Scene>();
                return scene->CreateEntity();
            }
            case ECSImplementation::Entt: 
                // Todo: Implement
            break;
        }
    }

    void ECSFace::DestroyEntity(uint64_t ent)
    {
        switch(s_ECSImplementation)
        {
            case ECSImplementation::InternalEcs: 
            {
                ecs::Scene* scene = ECSFace::CastTo<ecs::Scene>();
                scene->DestroyEntity(ent);
            }
            case ECSImplementation::Entt: 
                // Todo: Implement
            break;
        }
    }
}