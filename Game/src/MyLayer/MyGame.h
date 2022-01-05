#pragma once
#include "Entry/Layer.h"
#include "Render/GameCamera.h"
#include <unordered_map>
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Render/GL/FramebufferRender.h"

/* I use this so i dont need to create a method for each key on the MyGame class */
struct MyGameKeyBindings;
class MyGame;
/* Function pointer for store key bindigs */
typedef void (*MyGameKeyBindingFunctionPtr)(MyGame&);

class MyGame :
    public Game::Layer
{
protected:
	std::unordered_map<int, MyGameKeyBindingFunctionPtr> m_KeysBindings;
	Game::SceneCamera m_Camera;
	Game::Scope<Game::FramebufferRender> m_FrameBufferR;
public:
	MyGame(const std::string& name = "MyGame");
	virtual ~MyGame() = default;

	void OnAttach() override;
	void OnUpdate(Game::Timestamp dt)override;
	void OnImGuiRender() override {}
	void OnDetach()override;
	void OnEvent(Event& e)override;

	bool OnWindowResize(WindowResizeEvent& e);
	bool OnKeyEvent(KeyPressedEvent& e);

private:
	friend struct MyGameKeyBindings;
};



