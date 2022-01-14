#pragma once
#include "Assert.h"
#include <bitset>
/* https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html */
namespace ecs
{
	typedef unsigned int entityIndex;
	typedef unsigned int entityVersion;
	typedef unsigned long long entity;
	constexpr int MAX_COMPONENTS = 32;
	typedef std::bitset<MAX_COMPONENTS> ComponentMask;

	extern int g_ComponentCounter;

	template<class T>
	int GetID()
	{
		static int s_ComponentID = g_ComponentCounter++;
		return s_ComponentID;
	}

	inline entity CreateEntityId(entityIndex index, entityVersion version)
	{
		return ((entity)index << 32) | ((entity)version);
	}

	inline entityIndex GetEntityIndex(entity ent)
	{
		return ent >> 32;
	}

	inline entityVersion GetEntityVersion(entity ent)
	{
		return (entityVersion)ent;
	}

	inline bool IsEntityValid(entity ent)
	{
		return (ent >> 32) != entityIndex(-1);
	}

}
#define ECS_INVALID_ENTITY ::ecs::CreateEntityId(::ecs::entityIndex(-1),0)