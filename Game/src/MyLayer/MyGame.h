#pragma once
#include "Entry/Layer.h"
#include <unordered_map>
#include "Event/ApplicationEvent.h"
#include "Event/KeyEvent.h"
#include "Event/MouseEvent.h"
#include "Scene/Scene.h"
#include "Scene/Components.h"
#include "Scene/Entity.h"
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
	Game::Scope<Game::Scene> m_Scene;

	Game::Entity m_Camera;
	Game::Entity m_Quad;
	Game::Entity m_Platform;
public:
	MyGame(const std::string& name = "MyGame");
	virtual ~MyGame() = default;

	void OnAttach() override;
	void OnUpdate(Game::Timestamp dt)override;
	void OnImGuiRender() override;
	void OnDetach()override;
	void OnEvent(Event& e)override;

	bool OnWindowResize(WindowResizeEvent& e);
	bool OnKeyEvent(KeyPressedEvent& e);

private:
	friend struct MyGameKeyBindings;
};



