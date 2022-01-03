#include "pch.h"
#include "Log.h"

#include <iostream>

namespace Game
{
	std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	void Log::Init()
	{
		try
		{
			spdlog::set_pattern("%^[%T] %n: %v%$");
			s_CoreLogger = spdlog::stdout_color_mt("Base");
			s_CoreLogger->set_level(spdlog::level::trace);

			s_ClientLogger = spdlog::stdout_color_mt("App");
			s_ClientLogger->set_level(spdlog::level::trace);
		}
		catch (const std::exception& e)
		{
			std::cerr << "Something went wrong when trying to Init spdlog, Error: " << e.what() << std::endl;
		}
	}
}