#include "pch.h"
#include "GL_Types.h"
#include "Log/Log.h"
#include "glad/glad.h"
#include "Core/Core.h"
namespace Game
{
	unsigned int GetType(GL_Type type)
	{
		switch (type)
		{
		case GL_Type::UNSIGNED_INT:
			return GL_UNSIGNED_INT;
			break;
		default:
			GAME_CORE_ASSERT(false, "GL Type not reconized");
			break;
		}
	}
	unsigned int GetTarget(GL_Target type)
	{
		switch (type)
		{
		case GL_Target::TRIANGLES:
			return GL_TRIANGLES;
			break;
		case GL_Target::LINE:
			return GL_LINE;
			break;
		case GL_Target::LINES:
			return GL_LINES;
			break;
		default:
			GAME_CORE_ASSERT(false, "GL Target not reconized");
			break;
		}
	}

	unsigned int GL_SwitchClearCommand(GL_ClearCommand command)
	{
		switch (command)
		{
		case GL_ClearCommand::ClearColor:
			return GL_COLOR_BUFFER_BIT;
		case GL_ClearCommand::ClearDepthColor:
			return GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT;
		default:
			GAME_CORE_ASSERT(false, "Unknown GL Clear Command");
		}
		return 0;
	}

	unsigned int GL_SwitchUsage(GL_Usage usage)
	{
		switch (usage)
		{
		case GL_Usage::DYNAMIC:
			return GL_DYNAMIC_DRAW;
		case GL_Usage::STATIC:
			return GL_STATIC_DRAW;
		case GL_Usage::STREAM:
			return GL_STREAM_DRAW;
		default:
			GAME_CORE_ASSERT(false, "Unknow GL_Usage");
		}
		return 0;
	}
}