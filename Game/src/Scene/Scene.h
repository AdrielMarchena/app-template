#pragma once

#include "Core/Core.h"
#include "Render/GL/FramebufferRender.h"
#include "Ecs/ECSScene.h"
#include "Render/GameCamera.h"
namespace Game
{
	class Entity; // Forward declaration for nice friendship
	class Scene
	{
	public:
		Scene();
		~Scene() = default;

		Entity CreateEntity(const std::string& tag = std::string());

		void OnUpdate(float dt);
		void OnResize(int w, int h);

		void FramebufferSetScalor(float scalor);
		float FramebufferGetScalor() const;
		const std::unordered_map<std::string, FramebufferPostEffect>& FramebufferGetPostEffects() const;
		void FramebufferSetPostEffect(const std::string& effect_name);
	private:
		Scope<ecs::Scene> m_Registry;
		Scope<FramebufferRender> m_FramebufferRender;
		SceneCamera m_FramebufferCamera;
		friend class Entity;
	};
}



