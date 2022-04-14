#pragma once

#include "Log/Log.h"
#include "Application.h"
#include "Debug/Intrumentator.h"

extern Game::Application* Game::CreateApplication(int argc, char** argv);

int main(int argc, char** argv)
{
	try
	{
		Game::Log::Init();
		GAME_PROFILE_BEGIN_SESSION("initialization", "game_initialization_profile.json");
		Game::Application* game = Game::CreateApplication(argc, argv);
		GAME_PROFILE_END_SESSION();
		
		GAME_PROFILE_BEGIN_SESSION("running", "game_running_profile.json");
		Game::Application::m_AppInstance = game;
		GAME_PROFILE_END_SESSION();

		GAME_PROFILE_BEGIN_SESSION("shutdown", "game_shutdown_profile.json");
		game->Run();
		GAME_PROFILE_END_SESSION();
		delete game;
	}
	catch (const std::exception& ex)
	{
		std::cout << "Unhandled Exception: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
