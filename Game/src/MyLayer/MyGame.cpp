#include "pch.h"
#include "MyGame.h"

#include "Input/Keyboard.h"
#include "Render/Render2D.h"
#include "Entry/Application.h"

#include <glm/gtx/quaternion.hpp>
struct Transform
{
	glm::vec3 Translation = {0.0f,0.0f,0.0f};
	glm::vec3 Scale = {1.0f,1.0f,1.0f};
	glm::vec3 Rotation = {0.0f,0.0f,0.0f};

	glm::mat4 GetTransform() const
	{
		glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

		return glm::translate(glm::mat4(1.0f), Translation)
			* rotation
			* glm::scale(glm::mat4(1.0f), Scale);
	}

} s_CameraTransform, s_FrameBuffCameraT, s_QuadT;

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
	Game::FrameBufferRenderSpecification specs;
	auto& app = Game::Application::Get();

	specs.width = app.GetWindow().GetWidth();
	specs.height = app.GetWindow().GetHeight();
	specs.scale_factor = 1.0f;

	m_FrameBufferR = Game::MakeScope<Game::FramebufferRender>(specs);
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
	m_Camera.SetViewportSize(w, h);

	float sc = m_Camera.GetOrthographicSize();

	m_FrameBufferR->SetQuadPosition({ 0.0f,0.0f,0.0f });
	m_FrameBufferR->SetQuadScale({ sc * ar,sc,1.0f });
	m_FrameBufferR->SetQuadRotation({ 0.0f,0.0f,0.0f });
}

void MyGame::OnUpdate(Game::Timestamp dt)
{
	//TODO: Pass this render routine to other place
	m_FrameBufferR->BindFrameBuffer();
	
	m_FrameBufferR->SetGLViewport(true);

	Game::Render2D::Clear();
	Game::Render2D::BeginScene(m_Camera, s_CameraTransform.GetTransform());
	Game::Render2D::BeginBatch();

	Game::Render2D::DrawQuad(s_QuadT.GetTransform(), Game::Colors::White);

	Game::Render2D::EndBatch();
	Game::Render2D::Flush();

	m_FrameBufferR->UnbindFrameBuffer();

	m_FrameBufferR->SetGLViewport(false);
	// TODO: This should be another camera, but will do for now
	m_FrameBufferR->DrawFrameBuffer(m_Camera, s_CameraTransform.GetTransform());
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
	m_Camera.SetViewportSize(w, h);
	m_FrameBufferR->GetSpec().width = w;
	m_FrameBufferR->GetSpec().height = h;
	float ar = (float)w / (float)h;
	float sc = m_Camera.GetOrthographicSize();
	m_FrameBufferR->SetQuadScale({ sc * ar,sc,1.0f });
	m_FrameBufferR->InvalidateFrameBuffer(); // Recreate Framebuffer
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
