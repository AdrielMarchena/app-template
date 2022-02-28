#pragma once

#include "Core/Core.h"
#include "Render/GL/FramebufferRender.h"
#include "Ecs/ECSScene.h"
#include "Render/GameCamera.h"
//#include "Message/MessageBus.h"
class b2World;
namespace Game
{
	class Entity; // Forward declaration for nice friendship
	class MessageBus;
	class Scene
	{
	public:
		Scene();
		~Scene();

		Entity CreateEntity(const std::string& tag = std::string(), bool addMessangerComponent = true);

		void SceneBegin();
		void SceneEnd();

		void RuntimeInit();
		void RuntimeStop();

		void OnUpdate(float dt);
		void OnResize(int w, int h);

		void FramebufferSetScalor(float scalor);
		float FramebufferGetScalor() const;
		const std::unordered_map<std::string, FramebufferPostEffect>& FramebufferGetPostEffects() const;
		void FramebufferSetPostEffect(const std::string& effect_name);

	private:
		void CreatePhysicWorld();
		void DisposePhysicWorld();

	private:
		Scope<ecs::Scene> m_Registry;
		Scope<FramebufferRender> m_FramebufferRender;
		SceneCamera m_FramebufferCamera;
		MessageBus* m_MessageBus = nullptr;
		b2World* m_PhysicWorld = nullptr;
		friend class Entity;
	};
}



