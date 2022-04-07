#pragma once
#include "Core/Core.h"
#include "ECSFace.h"
#include "Scene.h"
#include "Components.h"
namespace Game
{
	class ECSFace;
	class Entity
	{
	public:
		Entity() = default;
		Entity(const Entity&) = default;
		~Entity() = default;

		 operator bool() const { return m_Handler > -1; }
	public:

		template<typename T, class... _Args>
		T& Add(_Args&&... args)
		{
			GAME_CORE_ASSERT(!Contain<T>(), "Entity already contain this component");
			//return m_Registry->Add<T>(m_Handler, std::forward<_Args>(args)...);
			return ECSFace::AddComponent<T>(m_Handler, std::forward<_Args>(args)...);
		}

		TransformComponent& GetTransformComponent()
		{
			GAME_CORE_ASSERT(Contain<TransformComponent>(), "Entity does not contain transform component");
			//return m_Registry->Get<TransformComponent>(m_Handler);
			return ECSFace::GetComponent<TransformComponent>(m_Handler);
		}

		IdComponent& GetIdComponent()
		{
			GAME_CORE_ASSERT(Contain<IdComponent>(), "Entity does not contain Id component");
			//return m_Registry->Get<IdComponent>(m_Handler);
			return ECSFace::GetComponent<IdComponent>(m_Handler);
		}

		TagComponent& GetTagComponent()
		{
			GAME_CORE_ASSERT(Contain<TagComponent>(), "Entity does not contain tag component");
			//return m_Registry->Get<TagComponent>(m_Handler);
			return ECSFace::GetComponent<TagComponent>(m_Handler);
		}

		template<typename T>
		T& Get()
		{
			GAME_CORE_ASSERT(Contain<T>(), "Entity does not contain this component");
			//return m_Registry->Get<T>(m_Handler);
			return ECSFace::GetComponent<T>(m_Handler);
		}

		template<typename T>
		void Remove()
		{
			GAME_CORE_ASSERT(Contain<T>(), "Entity does nor contain this component");
			//m_Registry->Remove<T>(m_Handler);
			ECSFace::RemoveComponent<T>(m_Handler);
		}

		template<typename T>
		bool Contain()
		{
			GAME_CORE_ASSERT(m_SceneP && m_Registry, "Scene or Registry pointer are invalid");
			//return m_Registry->Contain<T>(m_Handler);
			return ECSFace::ContainComponent<T>(m_Handler);
		}

		uint64_t GetHandler() { return m_Handler; }
	private:
		//Constructor used by Scene class
		Entity(uint64_t handler, Scene* scene)
			:m_Handler(handler), m_SceneP(scene), m_Registry(&(*scene->m_Registry))
		{
		}
	private:
		Scene* m_SceneP = nullptr;
		uint64_t m_Handler{0}; //TODO: This is not a valid invalid entity, but well
		ecs::Scene* m_Registry = nullptr;
		friend class Scene;
	};
}


