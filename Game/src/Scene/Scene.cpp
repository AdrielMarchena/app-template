#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Render/Render2D.h"
#include "Render/GameCamera.h"
#include "Ecs/SceneView.h"
#include "Entry/Application.h"	
#include "Colision/Colide.h"
#include "Message/BusNode.h"
#include "ScriptableClass.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"

namespace Game
{

	static inline b2BodyType Getb2BoxType(RigidBody2DComponent::BodyType type)
	{
		switch(type)
		{
			case RigidBody2DComponent::BodyType::Static: return b2BodyType::b2_staticBody;
			case RigidBody2DComponent::BodyType::Dynamic: return b2BodyType::b2_dynamicBody;
			case RigidBody2DComponent::BodyType::Kinematic: return b2BodyType::b2_kinematicBody;
		}
		GAME_CORE_ASSERT(false,"Unknow body type");
		return b2BodyType::b2_staticBody;
	}

	Ref<Scene> Scene::m_CurrentScene = nullptr; 

	Scene::Scene()
	{
		m_Registry = MakeScope<ecs::Scene>();

		FrameBufferRenderSpecification specs;

		auto& app = Application::Get();
		int w = app.GetWindow().GetWidth();
		int h = app.GetWindow().GetHeight();

		specs.width = w;
		specs.height = h;

		m_FramebufferRender = MakeScope<FramebufferRender>(specs);

		float ar = (float)w / (float)h;
		m_FramebufferCamera.SetViewportSize(w,h);
		m_FramebufferCamera.SetOrthographicNearClip(-1.0f);
		m_FramebufferCamera.SetOrthographicFarClip(10.0f);

		float sc = m_FramebufferCamera.GetOrthographicSize();
		m_FramebufferRender->SetQuadPosition({ 0.0f,0.0f,0.0f });
		m_FramebufferRender->SetQuadScale({ sc * ar,sc,1.0f });
		m_FramebufferRender->SetQuadRotation({ 0.0f,0.0f,0.0f });

		m_MessageBus = new MessageBus();
	}

	Scene::~Scene()
	{
		if(m_MessageBus)
			delete m_MessageBus;
		if(m_PhysicWorld)
			delete m_PhysicWorld;
	}

	void Scene::SceneBegin()
	{
		auto view = ecs::SceneView<NativeScriptComponent>(*m_Registry);
		for(auto e : view)
		{
			Entity ent = { e, this };
			auto& scriptComponent = ent.Get<NativeScriptComponent>();
			if(!scriptComponent.Instance)
			{
				scriptComponent.Instance = scriptComponent.CreateInstance();
				scriptComponent.Instance->m_Entity = ent;
				scriptComponent.Instance->OnAttach();
			}
		}
	}

	void Scene::SceneEnd()
	{
		auto view = ecs::SceneView<NativeScriptComponent>(*m_Registry);
		for(auto e : view)
		{
			Entity ent = { e, this };
			auto& scriptComponent = ent.Get<NativeScriptComponent>();
			if(scriptComponent.Instance)
			{
				scriptComponent.Instance->OnDestroy();
				scriptComponent.DisposeInstance(&scriptComponent);
			}
		}
	}

	void Scene::RuntimeInit()
	{
		CreatePhysicWorld();
	}

	void Scene::RuntimeStop()
	{
		DisposePhysicWorld();
	}

	bool Scene::IsRuntimeInit() const
	{
		return m_PhysicWorld != nullptr;
	}

	void Scene::CreatePhysicWorld()
	{
		if(m_PhysicWorld) return;
		m_PhysicWorld = new b2World({0.0f,-9.8f});
		auto view = ecs::SceneView<RigidBody2DComponent>(*m_Registry);
		for(auto e : view)
		{
			Entity ent = { e, this };
			auto& transformComponent = ent.GetTransformComponent();
			auto& rb2d = ent.Get<RigidBody2DComponent>();

			b2BodyDef bodyDef;

			bodyDef.type = Getb2BoxType(rb2d.Type);
			bodyDef.position.Set(transformComponent.Translation.x,transformComponent.Translation.y);
			bodyDef.angle = transformComponent.Rotation.z;

			b2Body* body = m_PhysicWorld->CreateBody(&bodyDef);
			body->SetFixedRotation(rb2d.FixedRotation);
			rb2d.RuntimeBody = body;

			if(ent.Contain<BoxColider2DComponent>())
			{
				auto& bc2d = ent.Get<BoxColider2DComponent>();

				b2PolygonShape boxShape;
				boxShape.SetAsBox(bc2d.Size.x * transformComponent.Scale.x, bc2d.Size.y * transformComponent.Scale.y);

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &boxShape;
				fixtureDef.density = bc2d.Density;
				fixtureDef.friction = bc2d.Friction;
				fixtureDef.restitution = bc2d.Restitution;
				fixtureDef.restitutionThreshold = bc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}
		}
	}

	void Scene::DisposePhysicWorld()
	{
		if(m_PhysicWorld)
		{
			delete m_PhysicWorld;
			m_PhysicWorld = nullptr;
		}
	}

	Entity Scene::CreateEntity(const std::string& tag, bool addMessangerComponent)
	{
		
		Entity ent{ m_Registry->CreateEntity(), this };

		ent.Add<IdComponent>();
		ent.Add<TagComponent>(tag.empty() ? "Unnamed Entity" : tag);
		ent.Add<TransformComponent>();
		if (addMessangerComponent)
			auto& mes = ent.Add<MessageComponent>(m_MessageBus);
		return ent;
	}

	void Scene::OnUpdate(float dt)
	{

		{// Scripts
			auto view = ecs::SceneView<NativeScriptComponent>(*m_Registry);
			for (auto e : view)
			{
				Entity ent{e, this};
				auto& scriptComponent = ent.Get<NativeScriptComponent>();
				if(scriptComponent.Instance)
					scriptComponent.Instance->OnUpdate(dt);
			}
		}

		if(m_PhysicWorld)
		{// Physics
			constexpr int32_t velocityIterations = 6;
			constexpr int32_t positionIterations = 2;

			m_PhysicWorld->Step(dt,velocityIterations,positionIterations);

			auto view = ecs::SceneView<RigidBody2DComponent>(*m_Registry);
			for (auto e : view)
			{
				Entity ent{e, this};
				auto& rb2d = ent.Get<RigidBody2DComponent>();

				auto& transformComponent = ent.GetTransformComponent();
				b2Body* body = (b2Body*)rb2d.RuntimeBody;
				const auto& position = body->GetPosition();
				transformComponent.Translation.x = position.x;
				transformComponent.Translation.y = position.y;
				transformComponent.Rotation.z = body->GetAngle();
			}

		}

		//Notify all messages
		m_MessageBus->Notify();

		SceneCamera* main_camera = nullptr;
		TransformComponent* main_camera_trasform = nullptr;
		{
			auto view = ecs::SceneView<CameraComponent>(*m_Registry);
			for (auto ent : view)
			{
				Entity camera = { ent, this };
				auto& tra = camera.GetTransformComponent();
				auto& cam = camera.Get<CameraComponent>();

				if (cam.Primary)
				{
					main_camera = &cam.Camera;
					main_camera_trasform = &tra;
				}

			}
		}

		{//Physics
			//TODO: Test this idk if works
			auto view = ecs::SceneView<BoxColiderComponent>(*m_Registry);
			
			static glm::vec2 gravity = { 0.0f, 9.8f };
			for (auto ent : view)
			{

				Entity boxEntity = { ent, this };
				auto& entTransform = boxEntity.GetTransformComponent();
				auto& entBox = boxEntity.Get<BoxColiderComponent>();
				
				glm::vec2 cp, cn;
				float ct = 0;
				std::vector<std::pair<Entity, float>> Colided;

				if (entBox.BodyType == BoxColiderComponent::Type::Dynamic)
				{
					ColisionRect entRect;
					entRect.pos = entTransform.Translation;
					entRect.size = entTransform.Scale;
					entBox.Velocity -= gravity * dt;
					entRect.velocity = entBox.Velocity;

					for (auto other : view)
					{
						if (other == ent)
							continue;
						Entity otherEntity = { other, this };
						auto& otherBox = otherEntity.Get<BoxColiderComponent>();
						if (otherBox.BodyType == BoxColiderComponent::Type::Dynamic)
							continue;

						auto& otherTransform = otherEntity.GetTransformComponent();
						
						ColisionRect otherRect;
						otherRect.pos = otherTransform.Translation;
						otherRect.size = otherTransform.Scale;

						if (Colide::DynamicRectVsRect(entRect, otherRect, cp, cn, ct, dt))
						{
							Colided.push_back({ otherEntity,ct });
						}

					}
					std::sort(Colided.begin(), Colided.end(), [](const std::pair<Entity, float>& a, const std::pair<Entity, float>& b)
						{
							return a.second < b.second;
						});

					for (auto& e : Colided)
					{
						auto& otherTransform = e.first.GetTransformComponent();
						ColisionRect otherRect;
						otherRect.pos = otherTransform.Translation;
						otherRect.size = otherTransform.Scale;

						if (Colide::DynamicRectVsRect(entRect, otherRect, cp, cn, ct, dt))
						{
							entBox.Velocity += cn * glm::vec2(std::abs(entBox.Velocity.x), std::abs(entBox.Velocity.y)) * (1 - ct);
						}
					}

					entTransform.Translation += glm::vec3(entBox.Velocity * dt, entTransform.Translation.z);
				}
			}
		}//Physics

		{//Render Scope
			m_FramebufferRender->BindFrameBuffer();
			Render2D::Clear();

			if (main_camera)
			{
				m_FramebufferRender->SetGLViewport(true);
				
				Render2D::BeginScene(*main_camera, *main_camera_trasform);
				Render2D::BeginBatch();
				
				auto view = ecs::SceneView<SpriteComponent>(*m_Registry);
				for (auto ent : view)
				{
					auto& sprite = m_Registry->Get<SpriteComponent>(ent);
					if (sprite.Visible)
					{
						auto& tra = m_Registry->Get<TransformComponent>(ent);
						if (sprite.Texture)
							Render2D::DrawQuad(tra.GetTransform(), sprite.Texture);
						else
							Render2D::DrawQuad(tra.GetTransform(), sprite.Color);
					}
				}

				Render2D::EndBatch();
				Render2D::Flush();
			}

			m_FramebufferRender->UnbindFrameBuffer();

			m_FramebufferRender->SetGLViewport(false);
			// This TransformComponent don't ever change (i think)
			static TransformComponent m_FramebufferCameraTransform;
			m_FramebufferRender->DrawFrameBuffer(m_FramebufferCamera, m_FramebufferCameraTransform);
		}
	}

	void Scene::OnResize(int w, int h)
	{
		auto& specs = m_FramebufferRender->GetSpec();
		specs.width = w;
		specs.height = h;
		float ar = (float)w / (float)h;
		float sc = m_FramebufferCamera.GetOrthographicSize();
		m_FramebufferRender->SetQuadScale({ sc * ar,sc,1.0f });
		m_FramebufferCamera.SetViewportSize(w, h);

		m_FramebufferRender->InvalidateFrameBuffer();
	}

	void Scene::FramebufferSetScalor(float scalor)
	{
		m_FramebufferRender->GetSpec().scale_factor = scalor;
		m_FramebufferRender->InvalidateFrameBuffer();
	}

	float Scene::FramebufferGetScalor() const
	{
		return m_FramebufferRender->GetSpec().scale_factor;
	}

	const std::unordered_map<std::string, FramebufferPostEffect>& Scene::FramebufferGetPostEffects() const
	{
		return m_FramebufferRender->GetPostEffects();
	}

	void Scene::FramebufferSetPostEffect(const std::string& effect_name)
	{
		m_FramebufferRender->UsePostEffect(effect_name);
	}

	void Scene::MakeCurrentSceneRef(const Ref<Scene>& scene)
	{
		Scene::m_CurrentScene = scene;
	}

	void Scene::InvalidateCurrentSceneRef()
	{
		Scene::m_CurrentScene.reset();
	}

	void Scene::SetEnableBody(Entity ent,bool flag)
	{
		if(ent.Contain<RigidBody2DComponent>())
		{
			auto& rb2d = ent.Get<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			body->SetEnabled(flag);
		}
	}

	bool Scene::GetEnableBody(Entity ent) const
	{
		if(ent && ent.Contain<RigidBody2DComponent>())
		{
			auto& rb2d = ent.Get<RigidBody2DComponent>();
			b2Body* body = (b2Body*)rb2d.RuntimeBody;
			return body->IsEnabled();
		}
		return false;
	}
}
