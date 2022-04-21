#include "pch.h"
#include "Entry/Application.h"
#include "Entry/Entry.h"
#include "EditorLayer.h"

class Editor : public Game::Application
{
private:
public:
	Editor(int argc, char** argv)
		: Game::Application(argc,argv)
	{
		PushLayer(new EditorLayer());
	}

	~Editor()
	{
	}
};

Game::Application* Game::CreateApplication(int argc, char** argv)
{
	return new Editor(argc, argv);
}
