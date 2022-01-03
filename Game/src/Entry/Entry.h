#pragma once

#include "Log/Log.h"
#include "Application.h"

extern Game::Application* Game::CreateApplication(int argc, char** argv);

int main(int argc, char** argv)
{
	try
	{
		Game::Log::Init();

		Game::Application* game = Game::CreateApplication(argc, argv);
		Game::Application::m_AppInstance = game;
		game->Run();
		delete game;
	}
	catch (const std::exception& ex)
	{
		std::cout << "Unhandled Exception: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
