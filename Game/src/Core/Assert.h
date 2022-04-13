#pragma once

#include "debugbreak.h"
#include "Log/Log.h"

#define GAME_CORE_ASSERT(expression,...) if(!(expression)) { GAME_LOG_WARN(__VA_ARGS__); debug_break(); }
#define GAME_WARN_IF(expression,...) if(expression) { GAME_LOG_WARN(__VA_ARGS__); }
#define GAME_WARN_IF_NOT(expression,...) if(!(expression)) { GAME_LOG_WARN(__VA_ARGS__); }
