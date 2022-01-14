#pragma once

#include "debugbreak.h"

#ifdef GAME_DEBUG

#define ECS_ASSERT(expression) if(!(expression)) debug_break()

#else

#define ECS_ASSERT(expression) 

#endif