#include "pch.h"
#include "EditorLayer.h"

#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Render/Render2D.h"
#include "Render/GL/FramebufferRender.h"
#include "Render/GameCamera.h"
#include "Render/GL/Texture.h"
#include "Entry/Application.h"
#include "Utils/Files.h"

#include <glm/gtx/quaternion.hpp>
#include "imgui.h"
#include "Message/BusNode.h"
#include "Utils/Image/ImageLoader.h"

static Game::Ref<Game::FMODSound> g_TestSound;
static Game::Ref<Game::FMODChannel> g_TestSoundChannel;
static std::vector<std::pair<std::string, std::string>> g_SoundDirs;

static inline void PlaySound(const std::string& name, bool loop, const Game::SoundsSystemFMOD& soundSystem)
{
	if (!g_SoundDirs.empty())
	{
		g_TestSound = soundSystem.CreateSoundRef(g_SoundDirs[0].second, 0);
		g_TestSoundChannel = g_TestSound->GetChannel();
		g_TestSoundChannel->SetPosition(0);
		g_TestSoundChannel->SetLoopCount((!loop) - 1);
		g_TestSoundChannel->SetVolume(0.75f);
		g_TestSoundChannel->Play();
	}
}

struct MyGameKeyBindings
{
	static void KeyW(EditorLayer& g)
	{
		if(g_TestSoundChannel)
			g_TestSoundChannel->UpOctave();
	}

	static void KeyA(EditorLayer& g)
	{
		if (g_TestSoundChannel)
			g_TestSoundChannel->DownSemitone();
	}

	static void KeyS(EditorLayer& g)
	{
		if (g_TestSoundChannel)
			g_TestSoundChannel->DownOctave();
	}

	static void KeyD(EditorLayer& g)
	{
		if (g_TestSoundChannel)
			g_TestSoundChannel->UpSemitone();
	}

	static void KeyR(EditorLayer& g)
	{
		if (!g.m_Scene->IsRuntimeInit())
		{
			auto& quadTr = g.m_Quad.GetTransformComponent();
			quadTr.Translation = { 0.0f,4.0f,0.0f };
			auto& circleTr = g.m_Circle.Get<Game::TransformComponent>();
			circleTr.Translation = { 2.0f,4.0f,0.0f };
		}
	}

	static void KeyESC(EditorLayer& g)
	{
		Game::Application::Get().Close();
	}
};

EditorLayer::EditorLayer(const std::string& name)
	:Game::Layer(name)
{
}

void EditorLayer::OnAttach()
{
	m_KeysBindings.emplace(GAME_KEY_W, &MyGameKeyBindings::KeyW);
	m_KeysBindings.emplace(GAME_KEY_A, &MyGameKeyBindings::KeyA);
	m_KeysBindings.emplace(GAME_KEY_S, &MyGameKeyBindings::KeyS);
	m_KeysBindings.emplace(GAME_KEY_D, &MyGameKeyBindings::KeyD);
	m_KeysBindings.emplace(GAME_KEY_R, &MyGameKeyBindings::KeyR);
	m_KeysBindings.emplace(GAME_KEY_ESCAPE, &MyGameKeyBindings::KeyESC);

	auto& app = Game::Application::Get();

	auto w = app.GetWindow().GetWidth();
	auto h = app.GetWindow().GetHeight();
	float ar = (float)w / (float)h;

	m_Scene = Game::MakeRef<Game::Scene>();
	Game::Scene::MakeCurrentSceneRef(m_Scene);

	m_Scene->OnResize(w, h);

	auto images = Game::LoadBatchAsync("assets/img");
	{
		m_Camera = m_Scene->CreateEntity("Main_Camera");

		auto& cameraComponent = m_Camera.Add<Game::CameraComponent>();
		cameraComponent.Camera.SetViewportSize(w, h);
		auto& tr = m_Camera.Get<Game::TransformComponent>();
		tr.Translation = { 0.0f,0.0f,0.0f };
		tr.Scale = { 1.0f,1.0f,1.0f };
		tr.Translation = { 0.0f,0.0f,0.0f };
		cameraComponent.Primary = true;
	}

	{
		m_Quad = m_Scene->CreateEntity("Quad");
		auto& tr = m_Quad.GetTransformComponent();
		auto& sprite = m_Quad.Add<Game::SpriteComponent>(
			images["big"],
			[&tr](Game::SpriteComponent sprite)	
			{
				float texture_ar = sprite.Texture->GetSize().x / sprite.Texture->GetSize().y;
				tr.SetScaleWithAr(texture_ar);
				tr.Translation = { 0.0f,4.0f,0.0f };
			});

		auto& rigBody = m_Quad.Add<Game::RigidBody2DComponent>();
		auto& boxColider = m_Quad.Add<Game::BoxColider2DComponent>();
		rigBody.Type = Game::RigidBody2DComponent::BodyType::Dynamic;
	}

	{
		m_Platform = m_Scene->CreateEntity("Platform");
		auto& tr = m_Platform.Get<Game::TransformComponent>();
		m_Platform.Add<Game::SpriteComponent>(images["shrekOnion"]);

		tr.Translation = { 0.0f,0.0f,0.0f };
		tr.Scale = { 14.0f,1.0f,1.0f };

		auto& rigBody = m_Platform.Add<Game::RigidBody2DComponent>();
		auto& boxColider = m_Platform.Add<Game::BoxColider2DComponent>();
		rigBody.Type = Game::RigidBody2DComponent::BodyType::Static;
	}

	{
		m_Circle = m_Scene->CreateEntity("Cirlce");
		m_Circle.Add<Game::SpriteComponent>((images["chalote"]));
		m_Circle.Add<Game::CircleComponent>();
		auto& tr = m_Circle.Get<Game::TransformComponent>();

		tr.Translation = { 2.0f,4.0f,0.0f };
		tr.Scale = { 1.0f,1.0f,1.0f };

		auto& rigBody = m_Circle.Add<Game::RigidBody2DComponent>();
		auto& boxColider = m_Circle.Add<Game::CircleColider2DComponent>();
		rigBody.Type = Game::RigidBody2DComponent::BodyType::Dynamic;
	}

	m_SelectedEntity = Game::Entity{};

	m_Scene->AddDoBeforeUnbindFramebuffer([&](Game::Scene* scene) -> void
		{
			if (Game::Mouse::isClicked(GAME_MOUSE_BUTTON_LEFT))
			{
				auto& app = Game::Application::Get();
				auto h = app.GetWindow().GetHeight();
				auto mousePosition = Game::Mouse::m_pos(h);
				uint64_t entityID = scene->ReadPixel(1, mousePosition.x, mousePosition.y);
				APP_INFO("entityID on pixel {0}x{1} is: {2}", mousePosition.x, mousePosition.y, entityID);
				if (entityID > 0 && entityID < 100)
					m_SelectedEntity = Game::Entity{ entityID, m_Scene.get() };
			}
		});

	g_SoundDirs = Game::Files::GetPairText("assets/sounds", ".ogg#.wav#.WAV");
}

void EditorLayer::OnUpdate(Game::Timestamp dt)
{
	auto& mes = m_Quad.Get<Game::MessageComponent>();
	if (Game::Keyboard::isClicked(GAME_KEY_S))
		mes.Node.Send({ &m_Quad,Game::MessageEvent::PLAYER_DIED });
	m_Scene->OnUpdate(dt);
}

void EditorLayer::OnImGuiRender()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("Options"))
		{
			static float scalor = 1.0f;
			if (ImGui::SliderFloat("Framebuffer Scaler", &scalor, 0.05f, 1.0f))
			{
				m_Scene->FramebufferSetScalor(scalor);
			}

			if (ImGui::Button("Exit")) Game::Application::Get().Close();

			ImGui::EndMenu();
		}

		//if (ImGui::BeginMenu("Post Effects"))
		//{
		//	auto& postEffects = m_Scene->FramebufferGetPostEffects();
		//
		//	if (ImGui::Button("none")) m_Scene->FramebufferSetPostEffect("none");
		//
		//	for (const auto& effect : postEffects)
		//	{
		//		if (ImGui::Button(effect.first.c_str()))
		//			m_Scene->FramebufferSetPostEffect(effect.first);
		//	}
		//
		//	ImGui::EndMenu();
		//}

		if (ImGui::BeginMenu("Load and play sounds"))
		{
			static bool loop = true;
			ImGui::Checkbox("Loop sound", &loop);

			for (const auto& sound : g_SoundDirs)
			{
				if (ImGui::Button(sound.first.c_str()))
					PlaySound(sound.second, loop, m_Scene->GetSoundSystem());
			}

			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Runtime"))
		{
			if (ImGui::Button("Start"))
			{
				m_Scene->RuntimeInit();
			}

			if (ImGui::Button("Stop"))
			{
				m_Scene->RuntimeStop();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		ImGui::Begin("Entity Physics");

		if (m_SelectedEntity)
			if (m_SelectedEntity.Contain<Game::RigidBody2DComponent>() && m_SelectedEntity.Contain<Game::BoxColider2DComponent>())
			{
				auto& ent_rbody = m_SelectedEntity.Get<Game::RigidBody2DComponent>();
				auto& ent_bcol = m_SelectedEntity.Get<Game::BoxColider2DComponent>();

				ImGui::SliderFloat("Friction", &ent_bcol.Friction, 0.0f, 1.0f);
				ImGui::SliderFloat("Density", &ent_bcol.Density, 0.0f, 1.0f);
				ImGui::SliderFloat("Restitution", &ent_bcol.Restitution, 0.0f, 1.0f);
				ImGui::SliderFloat("RestitutionThreshold", &ent_bcol.RestitutionThreshold, 0.0f, 1.0f);
			}
			else if (m_SelectedEntity.Contain<Game::CircleComponent>())
			{
				auto& circle = m_SelectedEntity.Get<Game::CircleComponent>();

				ImGui::SliderFloat("Thickness", &circle.Thick, 0.0f, 1.0f);

				if (m_SelectedEntity.Contain<Game::RigidBody2DComponent>() && m_SelectedEntity.Contain<Game::CircleColider2DComponent>())
				{
					auto& ent_rbody = m_SelectedEntity.Get<Game::RigidBody2DComponent>();
					auto& ent_ccol = m_SelectedEntity.Get<Game::CircleColider2DComponent>();

					ImGui::SliderFloat("Friction", &ent_ccol.Friction, 0.0f, 1.0f);
					ImGui::SliderFloat("Density", &ent_ccol.Density, 0.0f, 1.0f);
					ImGui::SliderFloat("Restitution", &ent_ccol.Restitution, 0.0f, 1.0f);
					ImGui::SliderFloat("RestitutionThreshold", &ent_ccol.RestitutionThreshold, 0.0f, 1.0f);
					ImGui::SliderFloat("Radius", &ent_ccol.Radius, 0.0f, 1.0f);
				}
			}

		ImGui::End();

	}

}

void EditorLayer::OnDetach()
{
	m_Scene->RuntimeStop();

	m_Scene->InvalidateCurrentSceneRef();
}

void EditorLayer::OnEvent(Event& e)
{
	EventDispatcher disp(e);

	disp.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(EditorLayer::OnWindowResize));
	disp.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(EditorLayer::OnKeyEvent));
}

bool EditorLayer::OnWindowResize(WindowResizeEvent& e)
{
	auto w = e.GetWidth();
	auto h = e.GetHeight();
	m_Camera.Get<Game::CameraComponent>().Camera.SetViewportSize(w, h);
	m_Scene->OnResize(w, h);
	return false;
}

bool EditorLayer::OnKeyEvent(KeyPressedEvent& e)
{
	if (m_KeysBindings.find(e.GetKeyCode()) != m_KeysBindings.end())
	{
		m_KeysBindings[e.GetKeyCode()](*this);
	}
	return false;
}
