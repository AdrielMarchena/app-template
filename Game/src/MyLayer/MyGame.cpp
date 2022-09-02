#include "pch.h"
#include "MyGame.h"

#include "Input/Keyboard.h"
#include "Input/Mouse.h"
#include "Render/Render2D.h"
#include "Render/GL/FramebufferRender.h"
#include "Render/GameCamera.h"
#include "Render/GL/Texture.h"
#include "Entry/Application.h"

#include <glm/gtx/quaternion.hpp>
#include "imgui.h"
#include "Message/BusNode.h"

struct MyGameKeyBindings
{
	static void KeyW(MyGame& g)
	{
		if(!g.m_Scene->IsRuntimeInit())
		{
			auto& tr = g.m_Quad.Get<Game::TransformComponent>();
			tr.Translation = { 0.0f,4.0f,0.0f };
		}
	}
	static void KeyA(MyGame& g)
	{
	}
	static void KeyS(MyGame& g)
	{
	}
	static void KeyD(MyGame& g)
	{
	}
	static void KeyESC(MyGame& g)
	{
		Game::Application::Get().Close();
	}
};

MyGame::MyGame(const std::string& name)
	:Game::Layer(name)
{
}

void MyGame::OnAttach()
{
	m_KeysBindings.emplace(GAME_KEY_W, &MyGameKeyBindings::KeyW);
	m_KeysBindings.emplace(GAME_KEY_A, &MyGameKeyBindings::KeyA);
	m_KeysBindings.emplace(GAME_KEY_S, &MyGameKeyBindings::KeyS);
	m_KeysBindings.emplace(GAME_KEY_D, &MyGameKeyBindings::KeyD);
	m_KeysBindings.emplace(GAME_KEY_ESCAPE, &MyGameKeyBindings::KeyESC);

	auto& app = Game::Application::Get();

	auto w = app.GetWindow().GetWidth();
	auto h = app.GetWindow().GetHeight();
	float ar = (float)w / (float)h;

	m_Scene = Game::MakeRef<Game::Scene>();
	Game::Scene::MakeCurrentSceneRef(m_Scene);

	m_Scene->OnResize(w, h);

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
		auto texture = Game::Texture::CreateTexture("assets/img/shrekOnion.jpg");
		m_Quad.Add<Game::SpriteComponent>(texture);
		auto& tr = m_Quad.GetTransformComponent();

		float texture_ar = texture->GetSize().x / texture->GetSize().y;
		tr.SetScaleWithAr(texture_ar);
		tr.Translation = { 0.0f,4.0f,0.0f };

		auto& rigBody = m_Quad.Add<Game::RigidBody2DComponent>();
		auto& boxColider = m_Quad.Add<Game::BoxColider2DComponent>();
		rigBody.Type = Game::RigidBody2DComponent::BodyType::Dynamic;
	}

	{
		m_Platform = m_Scene->CreateEntity("Platform");
		auto texture = Game::Texture::CreateTexture("assets/img/chalote.jpg");
		m_Platform.Add<Game::SpriteComponent>(texture);
		auto& tr = m_Platform.Get<Game::TransformComponent>();

		tr.Translation = { 0.0f,0.0f,0.0f };
		tr.Scale = { 10.0f,1.0f,1.0f };

		auto& rigBody = m_Platform.Add<Game::RigidBody2DComponent>();
		auto& boxColider = m_Platform.Add<Game::BoxColider2DComponent>();
		rigBody.Type = Game::RigidBody2DComponent::BodyType::Static;
	}

	m_Scene->AddDoBeforeUnbindFramebuffer([&](Game::Scene* scene) -> void
	{
			if (Game::Mouse::isClicked(GAME_MOUSE_BUTTON_LEFT))
			{
				auto& app = Game::Application::Get();
				auto h = app.GetWindow().GetHeight();
				auto mousePosition = Game::Mouse::m_pos(h);
				int entityID = scene->ReadPixel(1,mousePosition.x, mousePosition.y);
				GAME_LOG_INFO("entityID on pixel {0}x{1} is: {2}", mousePosition.x, mousePosition.y, entityID);
			}
	});

}

void MyGame::OnUpdate(Game::Timestamp dt)
{
	auto& mes = m_Quad.Get<Game::MessageComponent>();
	if(Game::Keyboard::isClicked(GAME_KEY_S))
		mes.Node.Send({ &m_Quad,Game::MessageEvent::PLAYER_DIED });
	m_Scene->OnUpdate(dt);
}

void MyGame::OnImGuiRender()
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

			if(ImGui::Button("Exit")) Game::Application::Get().Close();

			ImGui::EndMenu();
		}

		// if (ImGui::BeginMenu("Post Effects"))
		// {
		// 	auto& postEffects = m_Scene->FramebufferGetPostEffects();
		// 
		// 	for (const auto& effect : postEffects)
		// 	{
		// 		if (ImGui::Button(effect.first.c_str()))
		// 			m_Scene->FramebufferSetPostEffect(effect.first);
		// 	}
		// 
		// 	ImGui::EndMenu();
		// }

		if (ImGui::BeginMenu("Runtime"))
		{
			if(ImGui::Button("Start"))
			{
				m_Scene->RuntimeInit();
			}

			if(ImGui::Button("Stop"))
			{
				m_Scene->RuntimeStop();
			}
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

}

void MyGame::OnDetach()
{
	m_Scene->RuntimeStop();

	m_Scene->InvalidateCurrentSceneRef();
}

void MyGame::OnEvent(Event& e)
{
	EventDispatcher disp(e);

	disp.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(MyGame::OnWindowResize));
	disp.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(MyGame::OnKeyEvent));
}

bool MyGame::OnWindowResize(WindowResizeEvent& e)
{
	auto w = e.GetWidth();
	auto h = e.GetHeight();
	m_Camera.Get<Game::CameraComponent>().Camera.SetViewportSize(w,h);
	m_Scene->OnResize(w, h);
	return false;
}

bool MyGame::OnKeyEvent(KeyPressedEvent& e)
{
	if (m_KeysBindings.find(e.GetKeyCode()) != m_KeysBindings.end())
	{
		m_KeysBindings[e.GetKeyCode()](*this);
	}
	return false;
}
