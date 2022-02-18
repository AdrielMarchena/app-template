#pragma once

//TODO: Place the debugbreak file in the same dir
#include "debugbreak.h"

#ifdef GAME_DEBUG

#define ECS_ASSERT(expression) if(!(expression)) debug_break()

#else

#define ECS_ASSERT(expression) 

#endif