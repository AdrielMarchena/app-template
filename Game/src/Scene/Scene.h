#pragma once

#include "Core/Core.h"
#include "Render/GL/FramebufferRender.h"
#include "ECSFace.h"
#include "Ecs/ECSScene.h"
#include "Render/GameCamera.h"
#include "Sound/SoundsSystemFMOD.h"
#include <functional>
//#include "Message/MessageBus.h"
class b2World;
namespace Game
{
	class Scene;

	using DoBeforeUnbindFramebuffer = std::function<void(Scene*)>;
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

		bool IsRuntimeInit() const;

		void OnUpdate(float dt);
		void OnResize(int w, int h);

		void FramebufferSetScalor(float scalor);
		float FramebufferGetScalor() const;
		const std::unordered_map<std::string, FramebufferPostEffect>& FramebufferGetPostEffects() const;
		void FramebufferSetPostEffect(const std::string& effect_name);

		int ReadPixel(uint32_t index, int x, int y);
		void ClearAttachment(uint32_t index, int value);

		/* Holds a static Ref to a Scene */
		static void MakeCurrentSceneRef(const Ref<Scene>& scene);
		static Ref<Scene> GetCurrentSceneRef() { return Scene::m_CurrentScene; }
		/* Invalidate the static Ref to Scene */ 
		static void InvalidateCurrentSceneRef();

		/* enable or disable entity body */
		void SetEnableBody(Entity ent,bool flag);
		bool GetEnableBody(Entity ent) const;

		void AddDoBeforeUnbindFramebuffer(DoBeforeUnbindFramebuffer func);
		// void RemoveDoBeforeunbindFramebuffer();

		const SoundsSystemFMOD& GetSoundSystem() const { return m_SoundSystem; }
	private:
		void CreatePhysicWorld();
		void DisposePhysicWorld();

	private:
		std::vector<DoBeforeUnbindFramebuffer> m_FunctionsBeforeUnbindFramebuffer;
		Scope<ecs::Scene> m_Registry;
		Scope<FramebufferRender> m_FramebufferRender;
		SceneCamera m_FramebufferCamera;
		MessageBus* m_MessageBus = nullptr;
		b2World* m_PhysicWorld = nullptr;
		SoundsSystemFMOD m_SoundSystem;
		friend class Entity;
		static Ref<Scene> m_CurrentScene;
	};
}
