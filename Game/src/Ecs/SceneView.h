#pragma once

#include "General.h"
#include "ECSScene.h"
namespace ecs
{
	template<typename... _ComponentTypes>
	struct SceneView
	{
		Scene* pScene = nullptr;
		ComponentMask componentMask;
		bool all = false;

		SceneView(Scene& scene) : pScene(&scene)
		{
			//TODO: I really need study more about parameter packs
			if (sizeof...(_ComponentTypes) == 0)
				all = true;
			else
			{
				//My god, i really need to understand this better
				int componentIds[] = { 0, GetID<_ComponentTypes>()... };
				for (int i = 1; i < (sizeof...(_ComponentTypes) + 1); i++)
					componentMask.set(componentIds[i]);
			}
		}

		struct Iterator
		{
			Scene* pScene = nullptr;
			entityIndex index;
			ComponentMask mask;
			bool all = false;
			Iterator(Scene* scene, entityIndex index, ComponentMask mask, bool all)
				:pScene(scene), index(index), mask(mask), all(all)
			{}

			entity operator*() const
			{
				//give back the entity ew're currently at
				return pScene->entities[index].id;
			}

			bool operator==(const Iterator& other) const
			{
				//Compare two Iterators
				return index == other.index || index == pScene->entities.size();
			}

			bool operator!=(const Iterator& other) const
			{
				//Same as above, but different :p
				return index != other.index && index != pScene->entities.size();
			}

			bool ValidIndex()
			{
									//Check if is valid				//check mask component, or , if all is true, well, it's all
				return IsEntityValid(pScene->entities[index].id) && (all || mask == (mask & pScene->entities[index].mask));
			}

			Iterator& operator++()
			{
				do
				{
					index++;
				} while (index < pScene->entities.size() && !ValidIndex());
				return *this;
			}
		};

		const Iterator begin() const
		{
			//Give the iterator to the beginning
			int firstIndex = 0;
			while (firstIndex < pScene->entities.size() && (componentMask != (componentMask & pScene->entities[firstIndex].mask) || !IsEntityValid(pScene->entities[firstIndex].id)))
			{
				firstIndex++;
			}
			return Iterator(pScene, firstIndex, componentMask, all);
		}

		const Iterator end() const
		{
			//Give the iterator to the end
			return Iterator(pScene, entityIndex(pScene->entities.size()), componentMask, all);
		}

	};
}