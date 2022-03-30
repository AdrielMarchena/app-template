#pragma once
 
#include "Scene/Entity.h"
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

    static void CreateRegistry();

    //Ecs stuff
    static Entity CreateEntity();
    static void DestroyEntity(Entity ent);

    static ECSImplementation GetECSImplementation() { return s_ECSImplementation; }
    static std::string GetECSImplementationAsString()
    {
        if(s_ECSImplementation == ECSImplementation::Entt) return "Entt";
        if(s_ECSImplementation == ECSImplementation::InternalEcs) return "InternalEcs";
    }
};

}