#include "pch.h"
#include "Scene.h"
#include "ECSFace.h"
#include "Entity.h"
#include "Components.h"
#include "Render/Render2D.h"
#include "Render/GameCamera.h"
#include "Ecs/SceneView.h"
#include "Entry/Application.h"	
#include "Colision/Colide.h"
#include "Message/BusNode.h"
#include "ScriptableClass.h"
#include "Utils/Files.h"
#include "Debug/Intrumentator.h"
#include "Render/GL/VertexAttribute.h"
#include "Input/Mouse.h"

#include "box2d/b2_world.h"
#include "box2d/b2_body.h"
#include "box2d/b2_fixture.h"
#include "box2d/b2_polygon_shape.h"
#include "box2d/b2_circle_shape.h"

#include "fmod.hpp" // TODO: Temp

namespace Game
{

	//TODO: TEMPORARY
	struct LightShaderInfo
	{
		std::vector<LightComponent> Lights;
		glm::vec3 AmbientLightColor = { 1.0f, 1.0f, 1.0f };
		float AmbientStrength = 0.35f;
	};

	static LightShaderInfo g_SceneLights;

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
		GAME_PROFILE_FUNCTION();

		m_Registry = MakeScope<ecs::Scene>();

		FramebufferRenderSpecification specs;

		auto& app = Application::Get();
		specs.Width = app.GetWindow().GetWidth();
		specs.Height = app.GetWindow().GetHeight();

		m_FramebufferChainRender = MakeScope<FramebufferChainRender>(specs, specs);

		m_MessageBus = new MessageBus();
		m_ECSFace.CreateRegistry();

		//CreateLightChain();
		//CreateShadowChain();
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
		GAME_PROFILE_FUNCTION();

		auto view = m_ECSFace.View<NativeScriptComponent>();
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
		GAME_PROFILE_FUNCTION();

		auto view = m_ECSFace.View<NativeScriptComponent>();
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
		m_IsRuntime = true;
	}

	void Scene::RuntimeStop()
	{
		DisposePhysicWorld();
		m_IsRuntime = false;
	}

	bool Scene::IsRuntimeInit() const
	{
		return m_PhysicWorld != nullptr;
	}

	void Scene::CreatePhysicWorld()
	{
		GAME_PROFILE_FUNCTION();
		if(m_PhysicWorld) return;

		m_PhysicWorld = new b2World({0.0f,-9.8f});

		auto view = m_ECSFace.View<RigidBody2DComponent>();
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

			if (ent.Contain<CircleColider2DComponent>())
			{
				auto& cc2d = ent.Get<CircleColider2DComponent>();

				b2CircleShape circleShape;
				circleShape.m_p.SetZero();
				circleShape.m_radius = cc2d.Radius;

				b2FixtureDef fixtureDef;
				fixtureDef.shape = &circleShape;
				fixtureDef.density = cc2d.Density;
				fixtureDef.friction = cc2d.Friction;
				fixtureDef.restitution = cc2d.Restitution;
				fixtureDef.restitutionThreshold = cc2d.RestitutionThreshold;
				body->CreateFixture(&fixtureDef);
			}

		}
	}

	void Scene::DisposePhysicWorld()
	{
		GAME_PROFILE_FUNCTION();
		if(m_PhysicWorld)
		{
			delete m_PhysicWorld;
			m_PhysicWorld = nullptr;
		}
	}

	void Scene::CreateShadowChain()
	{
		auto& app = Application::Get();
		uint32_t w = app.GetWindow().GetWidth();
		uint32_t h = app.GetWindow().GetHeight();

		m_ShadowParse = Chain();
		Chain& chain = m_ShadowParse;

		chain.RenderData.FramebufferSpecifications.Width = w;
		chain.RenderData.FramebufferSpecifications.Height = h;
		chain.RenderData.FramebufferSpecifications.ScaleFactor = 1.0f;

		chain.RenderData.CShader = Shader::CreateShader("assets/shaders/Shadow.glsl");
		chain.RenderData.CShader->Bind();
		chain.RenderData.CShader->SetUniform1i("u_Framebuffer", 0);

		chain.RenderData.Frambuffer = FramebufferChainRender::CreateFramebuffer
		(
			chain.RenderData.FramebufferSpecifications.Width,
			chain.RenderData.FramebufferSpecifications.Height,
			chain.RenderData.FramebufferSpecifications.ScaleFactor
		);

		chain.RenderData.Buffer = new FramebufferQuad[4];
		chain.RenderData.BufferPtr = chain.RenderData.Buffer;

		chain.RenderData.VA = VertexArray::CreateVertexArray();

		chain.RenderData.VB = VertexBuffer::CreateVertexBuffer(sizeof(FramebufferQuad) * 4);

		VertexAttribute layout(chain.RenderData.VB);

		layout.AddLayoutFloat(3, sizeof(FramebufferQuad), (const void*)offsetof(FramebufferQuad, Position));
		layout.AddLayoutFloat(2, sizeof(FramebufferQuad), (const void*)offsetof(FramebufferQuad, TexCoords));

		uint32_t indices[6]{};
		uint32_t offset = 0;
		for (int i = 0; i < 6; i += 6)
		{
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		float ar = (float)w / (float)h;
		chain.RenderData.RenderCamera.SetViewportSize(w, h);
		chain.RenderData.RenderCamera.SetOrthographicNearClip(-1.0f);
		chain.RenderData.RenderCamera.SetOrthographicFarClip(10.0f);

		float sc = chain.RenderData.RenderCamera.GetOrthographicSize();

		chain.RenderData.Position = { 0.0f,0.0f,0.0f };
		chain.RenderData.Scale = { sc * ar,sc, 1.0f };
		chain.RenderData.Rotation = { 0.0f,0.0f,0.0f };

		FramebufferChainRender::CalculateQuadTransform(chain);
		chain.OnResizeFunc = FramebufferChainRender::StandardChainOnResizeCallback;

		chain.PreDrawFunc = [&](Chain& self, FramebufferChainRenderData&)
		{
			Render2D::SetBlendFunc(GLBlendFactor::SRC_ALPHA, GLBlendFactor::ONE_MINUS_SRC_ALPHA);
			self.RenderData.CShader->Bind();
			self.RenderData.CShader->SetUniform1f("u_AmbientStrength", g_SceneLights.AmbientStrength);
			self.RenderData.CShader->SetUniform3f("u_Shadow",
												  g_SceneLights.AmbientLightColor.x,
												  g_SceneLights.AmbientLightColor.y,
												  g_SceneLights.AmbientLightColor.z);
		};

		chain.RenderData.IB = IndexBuffer::CreateIndexBuffer(sizeof(indices), indices);
		chain.RenderData.VA->Unbind();
		m_FramebufferChainRender->AddChain(chain);
	}

	void Scene::CreateLightChain()
	{
		auto& app = Application::Get();
		uint32_t w = app.GetWindow().GetWidth();
		uint32_t h = app.GetWindow().GetHeight();

		m_ShadowParse = Chain();
		Chain& chain = m_ShadowParse;

		chain.RenderData.FramebufferSpecifications.Width = w;
		chain.RenderData.FramebufferSpecifications.Height = h;
		chain.RenderData.FramebufferSpecifications.ScaleFactor = 1.0f;

		chain.RenderData.CShader = Shader::CreateShader("assets/shaders/Light.glsl");
		chain.RenderData.CShader->Bind();
		chain.RenderData.CShader->SetUniform1i("u_Framebuffer", 0);

		chain.RenderData.Frambuffer = FramebufferChainRender::CreateFramebuffer
		(
			chain.RenderData.FramebufferSpecifications.Width,
			chain.RenderData.FramebufferSpecifications.Height,
			chain.RenderData.FramebufferSpecifications.ScaleFactor
		);

		chain.RenderData.Buffer = new FramebufferQuad[4];
		chain.RenderData.BufferPtr = chain.RenderData.Buffer;

		chain.RenderData.VA = VertexArray::CreateVertexArray();

		chain.RenderData.VB = VertexBuffer::CreateVertexBuffer(sizeof(FramebufferQuad) * 4);

		VertexAttribute layout(chain.RenderData.VB);

		layout.AddLayoutFloat(3, sizeof(FramebufferQuad), (const void*)offsetof(FramebufferQuad, Position));
		layout.AddLayoutFloat(2, sizeof(FramebufferQuad), (const void*)offsetof(FramebufferQuad, TexCoords));

		uint32_t indices[6]{};
		uint32_t offset = 0;
		for (int i = 0; i < 6; i += 6)
		{
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;

			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		float ar = (float)w / (float)h;
		chain.RenderData.RenderCamera.SetViewportSize(w, h);
		chain.RenderData.RenderCamera.SetOrthographicNearClip(-1.0f);
		chain.RenderData.RenderCamera.SetOrthographicFarClip(10.0f);

		float sc = chain.RenderData.RenderCamera.GetOrthographicSize();

		chain.RenderData.Position = { 0.0f,0.0f,0.0f };
		chain.RenderData.Scale = { sc * ar,sc, 1.0f };
		chain.RenderData.Rotation = { 0.0f,0.0f,0.0f };

		FramebufferChainRender::CalculateQuadTransform(chain);
		chain.OnResizeFunc = FramebufferChainRender::StandardChainOnResizeCallback;

		chain.PreDrawFunc = [&](Chain& self, FramebufferChainRenderData&)
		{
			self.RenderData.CShader->Bind();
			self.RenderData.CShader->SetUniform4f("u_Color2", 0.0f, 0.0f, 0.0f, 1.0f);
			self.RenderData.CShader->SetUniform4f("u_Color1", 1.0f, 1.0f, 1.0f, 1.0f);
		};

		chain.RenderData.IB = IndexBuffer::CreateIndexBuffer(sizeof(indices), indices);
		chain.RenderData.VA->Unbind();
		m_FramebufferChainRender->AddChain(chain);
	}

	Entity Scene::CreateEntity(const std::string& tag, bool addMessangerComponent)
	{
		GAME_PROFILE_FUNCTION();
		Entity ent{ m_ECSFace.CreateEntity(), this };

		ent.Add<IdComponent>();
		ent.Add<TagComponent>(tag.empty() ? "Unnamed Entity" : tag);
		ent.Add<TransformComponent>();
		if (addMessangerComponent)
			auto& mes = ent.Add<MessageComponent>(m_MessageBus);
		return ent;
	}

	void Scene::OnUpdate(float dt)
	{
		GAME_PROFILE_FUNCTION();

		auto& app = Application::Get();
		uint32_t w = app.GetWindow().GetWidth();
		uint32_t h = app.GetWindow().GetHeight();

		{// Scripts
			GAME_PROFILE_SCOPE("updating_scripts");

			auto view = m_ECSFace.View<NativeScriptComponent>();
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
			GAME_PROFILE_SCOPE("updating_physics");

			constexpr int32_t velocityIterations = 6;
			constexpr int32_t positionIterations = 2;

			m_PhysicWorld->Step(dt,velocityIterations,positionIterations);

			auto view = m_ECSFace.View<RigidBody2DComponent>();
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

		{ // Sound
			GAME_PROFILE_SCOPE("updating_sounds");
			m_SoundSystem.Update(dt);
		}

		SceneCamera* main_camera = nullptr;
		TransformComponent* main_camera_trasform = nullptr;
		{ // Find Camera
			GAME_PROFILE_SCOPE("find_camera");
			// auto view = ecs::SceneView<CameraComponent>(*m_Registry);
			auto view = m_ECSFace.View<CameraComponent>();
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
			GAME_PROFILE_SCOPE("opt_physics(unused)");
			//TODO: Test this idk if works
			// auto view = ecs::SceneView<BoxColiderComponent>(*m_Registry);
			auto view = m_ECSFace.View<BoxColiderComponent>();
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
			GAME_PROFILE_SCOPE("rendering");

			m_FramebufferChainRender->BindSceneFramebuffer();
			Render2D::Clear();

			GAME_DO_IF_ENTITYID(ClearAttachment(1, -1));

			if (main_camera)
			{
				Render2D::BeginScene(*main_camera, *main_camera_trasform);
				Render2D::BeginBatch();
				
				auto view = m_ECSFace.View<SpriteComponent>();
				for (auto ent : view)
				{
					auto& sprite = m_ECSFace.GetComponent<SpriteComponent>(ent);
					if (sprite.Visible)
					{
						auto& tra = m_ECSFace.GetComponent<TransformComponent>(ent);

						if (m_ECSFace.ContainComponent<CircleComponent>(ent)) // Draw as Circle Shape
						{
							auto& circleComponent = m_ECSFace.GetComponent<CircleComponent>(ent);
							if (sprite.Texture)
								Render2D::DrawCircle(tra.GetTransform(), circleComponent.Thick, circleComponent.Fade, sprite.Texture, sprite.Color GAME_COMMA_ENTITYID(m_ECSFace.GetEntityNumber(ent)));
							else
								Render2D::DrawCircle(tra.GetTransform(), circleComponent.Thick, circleComponent.Fade, sprite.Color GAME_COMMA_ENTITYID(m_ECSFace.GetEntityNumber(ent)));
						}
						else // Quad shapes
						{
							if (sprite.Texture)
								Render2D::DrawQuad(tra.GetTransform(), sprite.Texture, sprite.Color GAME_COMMA_ENTITYID(m_ECSFace.GetEntityNumber(ent)));
							else
								Render2D::DrawQuad(tra.GetTransform(), sprite.Color GAME_COMMA_ENTITYID(m_ECSFace.GetEntityNumber(ent)));
						}
					}
				}
				
				float csize = main_camera->GetOrthographicSize();
				auto screenMousePosition = glm::vec3(Mouse::screen_pos(w, h, csize), 1.0f);

				glm::vec3 v = glm::vec3(Mouse::m_pos(h), 0.0f);
				Render2D::DrawLine({ 0.0f,0.0f,0.0f }, screenMousePosition, { 1.0f,1.0f,1.0f,1.0f } GAME_COMMA_ENTITYID(0));

				Render2D::EndBatch();
				Render2D::Flush();
			}

			{// Light Map
				auto view = m_ECSFace.View<LightComponent>();
				for (auto& ent : view)
				{
					auto& tra = m_ECSFace.GetComponent<TransformComponent>(ent);
					uint32_t x = tra.Translation.x;
					uint32_t y = tra.Translation.y;
					auto& light = m_ECSFace.GetComponent<LightComponent>(ent);
				}
			}
			
			 for (auto& f : m_FunctionsBeforeUnbindFramebuffer)
			 	f(this);

			m_FramebufferChainRender->RenderChain();
		}
	}

	void Scene::OnResize(int w, int h)
	{
		GAME_PROFILE_FUNCTION();
		m_FramebufferChainRender->OnResize(w, h);
	}

	void Scene::FramebufferSetScalor(float scalor)
	{
		// m_FramebufferChainRender->SetScalorFactor(scalor);
	}

	float Scene::FramebufferGetScalor() const
	{
		// return m_FramebufferChainRender->GetScalorFactor();
		return 1.0f;
	}

	//const std::unordered_map<std::string, FramebufferPostEffect>& Scene::FramebufferGetPostEffects() const
	//{
	//	return m_FramebufferRender->GetPostEffects();
	//}

	//void Scene::FramebufferSetPostEffect(const std::string& effect_name)
	//{
	//	m_FramebufferRender->UsePostEffect(effect_name);
	//}

	int Scene::ReadPixel(uint32_t index, int x, int y)
	{
		return m_FramebufferChainRender->ReadPixel(index, x, y);
	}

	void Scene::ClearAttachment(uint32_t index, int value)
	{
		m_FramebufferChainRender->ClearAttachment(index, value);
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

	void Scene::AddDoBeforeUnbindFramebuffer(DoBeforeUnbindFramebuffer func)
	{
		if (func)
			m_FunctionsBeforeUnbindFramebuffer.push_back(func);

		// GAME_LOG_WARN("The function '{0}' is invalid", func);
	}
}
