#pragma once

#include "Log/Log.h"
#include "Application.h"
#include "Debug/Intrumentator.h"

#ifdef GAME_DEBUG

#define BENCHMARKING_CONFIGURATION "Debug/"

#elif GAME_RELEASE

#define BENCHMARKING_CONFIGURATION "Release/"

#else

#define BENCHMARKING_CONFIGURATION ""

#endif

extern Game::Application* Game::CreateApplication(int argc, char** argv);

int main(int argc, char** argv)
{
	try
	{
		Game::Log::Init();

		std::string benchmarkingDir = "benchmarking/" + std::string(BENCHMARKING_CONFIGURATION);

		GAME_PROFILE_BEGIN_SESSION("initialization", benchmarkingDir + "game_initialization_profile.json");
		Game::Application* game = Game::CreateApplication(argc, argv);
		Game::Application::m_AppInstance = game;
		GAME_PROFILE_END_SESSION();

		GAME_PROFILE_BEGIN_SESSION("running", benchmarkingDir + "game_running_profile.json");
		game->Run();
		GAME_PROFILE_END_SESSION();

		GAME_PROFILE_BEGIN_SESSION("shutdown", benchmarkingDir + "game_shutdown_profile.json");
		delete game;
		GAME_PROFILE_END_SESSION();
	}
	catch (const std::exception& ex)
	{
		std::cout << "Unhandled Exception: " << ex.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
