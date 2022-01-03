#include "pch.h"

#include "Entry/Entry.h" //Necessary to hit the right main function
#include "Entry/Application.h"

class Main : public Game::Application
{
public:
	Main(int argc, char** argv)
		: Game::Application(argc, argv)
	{
		//PushLayer
	}
};

Game::Application* Game::CreateApplication(int argc, char** argv)
{
	return new Main(argc, argv);
}