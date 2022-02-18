#pragma once

#include "General.h"
#include "ComponentPool.h"
#include <vector>

/* This is allocationg memory in a way that it has enough memory
	for each entity contain each component, if you want a robust
	ecs, i recommend using entt (search on google), but for a small game,
	this is fine
*/

/* https://www.david-colson.com/2020/02/09/making-a-simple-ecs.html */
namespace ecs
{
	struct Scene
	{
		struct EntityDesc
		{
			entity id;
			ComponentMask mask;
		};
		std::vector<EntityDesc> entities;

		~Scene()
		{
			for (auto pointer : componentPools)
			{
				if (pointer)
					delete pointer;
			}
		}

		entity CreateEntity()
		{
			if (!freeEntities.empty())
			{
				entityIndex newIndex = freeEntities.back();
				freeEntities.pop_back();
				entity newEnt = CreateEntityId(newIndex, GetEntityVersion(entities[newIndex].id));
				entities[newIndex].id = newEnt;
				return entities[newIndex].id;
			}

			entities.push_back(
				{ 
					CreateEntityId(entityIndex(entities.size()),0),
					ComponentMask() 
				});

			return entities.back().id;
		}

		void DestroyEntity(entity ent)
		{
			entity newEnt = CreateEntityId(entityIndex(-1), GetEntityVersion(ent) + 1);

			entities[GetEntityIndex(ent)].id = newEnt;
			entities[GetEntityIndex(ent)].mask.reset();
			freeEntities.push_back(GetEntityIndex(ent));
			
		}

		template<typename T, class... _Args>
		T& Add(entity ent,_Args&&... args)
		{
			int ComponentID = GetID<T>();
			ent = GetEntityIndex(ent);

			if (componentPools.size() <= ComponentID) // not enough space
			{
				componentPools.resize(ComponentID + 1, nullptr);
			}

			if (componentPools[ComponentID] == nullptr) // new component
			{
				componentPools[ComponentID] = new ComponentPool(sizeof(T));
			}

			T* pComponent = new (componentPools[ComponentID]->get(ent)) T(std::forward<_Args>(args)...);

			entities[ent].mask.set(ComponentID);
			return *pComponent;
		}

		template<typename T>
		T& Get(entity ent)
		{
			int ComponentID = GetID<T>();
			ent = GetEntityIndex(ent);
			ECS_ASSERT(entities[ent].mask.test(ComponentID));
			
			T* pComponent = static_cast<T*>(componentPools[ComponentID]->get(ent));
			return *pComponent;
		}

		template<typename T>
		bool Contain(entity ent)
		{
			int ComponentID = GetID<T>();
			ent = GetEntityIndex(ent);
			return entities[ent].mask.test(ComponentID);
		}

		template<typename T>
		void Remove(entity ent)
		{
			ECS_ASSERT(!(entities[GetEntityIndex(ent)].id != ent));

			if (entities[GetEntityIndex(ent)].id != ent)
				return;

			int ComponentID = GetId<T>();
			ent = GetEntityIndex(ent);
			entities[ent].mask.reset(ComponentID);
		}

		std::vector<ComponentPool*> componentPools;
		std::vector<entityIndex> freeEntities;
	};

}

