#pragma once

#include "UUID.h"
#include "Render/GL/Texture.h"
#include "Render/GameCamera.h"
#include "glm/glm.hpp"
#include "glm/gtx/quaternion.hpp"
#include "Sound/SoundsSystemFMOD.h"
//#include "Message/BusNode.h"
namespace Game
{
	void InitMetaReflectionComponents();

	struct TestComponent
	{
		int a = 256;
	};

	struct IdComponent
	{
		UUID Id;

		IdComponent() = default;
		IdComponent(const IdComponent&) = default;
	};

	struct TagComponent
	{
		std::string Tag;
		TagComponent() = default;
		TagComponent(const TagComponent&) = default;
		TagComponent(const std::string& tag)
			:Tag(tag)
		{}
	};

	struct SpriteComponent
	{
		glm::vec4 Color{ 1.0f,1.0f,1.0f,1.0f };
		bool Visible = true;
		Ref<Game::Texture> Texture;//Conflict name if above constructors

		SpriteComponent() = default;
		SpriteComponent(const SpriteComponent&) = default;

		SpriteComponent(const glm::vec4& color)
			:Color(color){}
		SpriteComponent(const Ref<Game::Texture>& texture)
			:Texture(texture) {}

	};

	struct TransformComponent
	{
		glm::vec3 Translation{ 0.0f,0.0f,0.0f };
		glm::vec3 Scale{ 1.0f,1.0f,1.0f };
		glm::vec3 Rotation{ 0.0f,0.0f,0.0f };

		TransformComponent() = default;
		TransformComponent(const TransformComponent&) = default;
		TransformComponent(const glm::vec3& translation):Translation(translation) {};

		glm::mat4 GetTransform() const
		{
			glm::mat4 rot = glm::toMat4(glm::quat(Rotation));
			return glm::translate(glm::mat4(1.0f), Translation) * rot * glm::scale(glm::mat4(1.0f), Scale);
		}

		/* It just multiply the Scale x value with the ar parameter */
		void SetScaleWithAr(float ar)
		{
			Scale.x *= ar;
		}

		operator glm::mat4() const { return GetTransform(); }
	};

	struct CircleComponent
	{
		float Thick = 1.0f;
		float Fade = 0.005f;

		CircleComponent() = default;
		CircleComponent(const CircleComponent&) = default;
	};

	struct RigidBody2DComponent
	{
		enum class BodyType
		{
			Static = 0, 
			Dynamic, 
			Kinematic
		} Type = BodyType::Static;

		bool FixedRotation = false;
		void* RuntimeBody = nullptr;

		RigidBody2DComponent() = default;
		RigidBody2DComponent(const RigidBody2DComponent&) = default;

	};

	struct BoxColider2DComponent
	{
		glm::vec2 Offset = { 0.0f, 0.0f };
		glm::vec2 Size = { 0.5f, 0.5f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;

		void* RuntimeFixture = nullptr;

		BoxColider2DComponent() = default;
		BoxColider2DComponent(const BoxColider2DComponent&) = default;
	};

	struct BoxColiderComponent
	{
		enum class Type
		{
			Dynamic,
			Static
		} BodyType = Type::Static;

		glm::vec2 Velocity = { 0.0f,0.0f };

		BoxColiderComponent() = default;
		BoxColiderComponent(const BoxColiderComponent&) = default;

	};

	struct CircleColider2DComponent
	{
		glm::vec2 Offset = { 0.0f,0.0f };
		glm::vec2 Size = { 0.5f,0.5f };

		float Density = 1.0f;
		float Friction = 0.5f;
		float Restitution = 0.0f;
		float RestitutionThreshold = 0.5f;
		float Radius = 0.5f;

		void* RuntimeFixture = nullptr;

		CircleColider2DComponent() = default;
		CircleColider2DComponent(const CircleColider2DComponent&) = default;
	};

	struct CameraComponent
	{
		SceneCamera Camera;
		bool Primary = true;

		CameraComponent() = default;
		CameraComponent(const CameraComponent&) = default;
	};

	struct ScriptableClass;
	struct NativeScriptComponent
	{
		ScriptableClass* Instance;
		ScriptableClass* (*CreateInstance)();
		void(*DisposeInstance)(NativeScriptComponent*);

		template<typename T>
		void Bind()
		{
			CreateInstance = []() { return static_cast<ScriptableClass*>(new T()); };
			DisposeInstance = [](NativeScriptComponent* script) { if (script->Instance) { delete script->Instance; script->Instance = nullptr; } };
		}
	};
}