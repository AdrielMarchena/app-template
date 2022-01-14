#include "pch.h"
#include "MyGame.h"

#include "Input/Keyboard.h"
#include "Render/Render2D.h"
#include "Render/GL/FramebufferRender.h"
#include "Render/GameCamera.h"
#include "Render/GL/Texture.h"
#include "Entry/Application.h"

#include <glm/gtx/quaternion.hpp>
#include "imgui.h"

struct MyGameKeyBindings
{
	static void KeyW(MyGame& g)
	{
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

	m_Scene = Game::MakeScope<Game::Scene>();

	m_Scene->OnResize(w, h);

	//m_Camera.SetViewportSize(w, h);

	{
		m_Camera = m_Scene->CreateEntity("Main_Camera");

		auto& cameraComponent = m_Camera.Add<Game::CameraComponent>();
		cameraComponent.Camera.SetViewportSize(w, h);
		//cameraComponent.Camera.SetOrthographicNearClip(-1.0f);
		//cameraComponent.Camera.SetOrthographicFarClip(10.0f);
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
		auto& tr = m_Quad.Get<Game::TransformComponent>();

		float texture_ar = texture->GetSize().x / texture->GetSize().y;
		tr.SetScaleWithAr(texture_ar);
	}

}

void MyGame::OnUpdate(Game::Timestamp dt)
{
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

		if (ImGui::BeginMenu("Post Effects"))
		{
			auto& postEffects = m_Scene->FramebufferGetPostEffects();

			for (const auto& effect : postEffects)
			{
				if (ImGui::Button(effect.first.c_str()))
					m_Scene->FramebufferSetPostEffect(effect.first);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

}

void MyGame::OnDetach()
{
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