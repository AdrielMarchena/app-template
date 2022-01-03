#pragma once

#include <memory>
#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
namespace Game
{
class Log
{
public:
	static void Init();

	inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() { return s_CoreLogger; }
	inline static std::shared_ptr<spdlog::logger>& GetClientLogger() { return s_ClientLogger; }
private:
	static std::shared_ptr<spdlog::logger> s_CoreLogger;
	static std::shared_ptr<spdlog::logger> s_ClientLogger;
};
}

#if defined GAME_DEBUG || defined GAME_RELEASE //Debug / Release defs

//Core Errors macros
/* Macro for loggin engine error's */
#define GAME_LOG_ERROR(...) ::Game::Log::GetCoreLogger()->error(__VA_ARGS__)
/* Macro for loggin engine warning's */
#define GAME_LOG_WARN(...)  ::Game::Log::GetCoreLogger()->warn(__VA_ARGS__)
/* Macro for loggin engine info's */
#define GAME_LOG_INFO(...)  ::Game::Log::GetCoreLogger()->info(__VA_ARGS__)
/* Macro for loggin engine debug info */
#define GAME_LOG_DEBUG_LOG(...) ::Game::Log::GetCoreLogger()->debug(__VA_ARGS__)
/* Macro for loggin engine trace */
#define GAME_LOG_TRACE(...) ::Game::Log::GetCoreLogger()->trace(__VA_ARGS__)
/* Macro for loggin engine fatal(errors probably) */
#define GAME_LOG_FATAL(...) ::Game::Log::GetCoreLogger()->fatal(__VA_ARGS__)

//Application Errors macros
/* Macro for loggin application/game error's */
#define APP_ERROR(...) ::Game::Log::GetClientLogger()->error(__VA_ARGS__)
/* Macro for loggin application/game wanning's */
#define APP_WARN(...)  ::Game::Log::GetClientLogger()->warn(__VA_ARGS__)
/* Macro for loggin application/game info's */
#define APP_INFO(...)  ::Game::Log::GetClientLogger()->info(__VA_ARGS__)
/* Macro for loggin application/game debug info */
#define APP_DEBUG(...) ::Game::Log::GetClientLogger()->debug(__VA_ARGS__)
/* Macro for loggin application/game trace */
#define APP_TRACE(...) ::Game::Log::GetClientLogger()->trace(__VA_ARGS__)
/* Macro for loggin application/game fatal(errors probably) */
#define APP_FATAL(...) ::Game::Log::GetClientLogger()->fatal(__VA_ARGS__)

#else //Dist defs

//Core Errors macros
/* Macro for loggin engine error's */
#define GAME_LOG_ERROR(...) 
/* Macro for loggin engine warning's */
#define GAME_LOG_WARN(...) 
/* Macro for loggin engine info's */
#define GAME_LOG_INFO(...) 
/* Macro for loggin engine debug info */
#define GAME_LOG_DEBUG_LOG(...) 
/* Macro for loggin engine trace */
#define GAME_LOG_TRACE(...) 
/* Macro for loggin engine fatal(errors probably) */
#define GAME_LOG_FATAL(...) 

//Application Errors macros
/* Macro for loggin application/game error's */
#define APP_ERROR(...) 
/* Macro for loggin application/game wanning's */
#define APP_WARN(...) 
/* Macro for loggin application/game info's */
#define APP_INFO(...) 
/* Macro for loggin application/game debug info */
#define APP_DEBUG(...) 
/* Macro for loggin application/game trace */
#define APP_TRACE(...) 
/* Macro for loggin application/game fatal(errors probably) */
#define APP_FATAL(...) 

#endif
