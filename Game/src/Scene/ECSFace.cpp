#include "pch.h"
#include "ECSFace.h"

namespace Game
{
    ECSImplementation ECSFace::s_ECSImplementation = ECSImplementation::Entt;
    void* ECSFace::m_Registry = nullptr;

    
}