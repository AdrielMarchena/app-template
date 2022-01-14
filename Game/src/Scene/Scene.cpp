#include "pch.h"
#include "Scene.h"
#include "Entity.h"
#include "Components.h"
#include "Render/Render2D.h"
#include "Render/GameCamera.h"
#include "Ecs/SceneView.h"
#include "Entry/Application.h"	
namespace Game
{
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

	}

	Entity Scene::CreateEntity(const std::string& tag)
	{
		Entity ent{ m_Registry->CreateEntity(), this };

		ent.Add<IdComponent>();
		ent.Add<TagComponent>(tag.empty() ? "Unnamed Entity" : tag);
		ent.Add<TransformComponent>();
		return ent;
	}

	void Scene::OnUpdate(float dt)
	{
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
}