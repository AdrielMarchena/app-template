#pragma once
#include <stdint.h>

//TODO: Remove typedefs
//typedef unsigned int GLenum;
//typedef int GLsizei;
//typedef unsigned int GLuint;
//typedef ptrdiff_t GLsizeiptr;
namespace Game
{
	//Data types
	enum class GL_Type: uint8_t
	{
		UNSIGNED_INT = 0
	};

	//Enum types
	enum class GL_Target: uint8_t
	{
		TRIANGLES = 0,
		LINES,
		LINE
	};

	enum class GL_Usage : uint8_t
	{
		DYNAMIC = 1,
		STATIC,
		STREAM
	};

	enum class GL_ClearCommand : uint8_t
	{
		ClearColor = 1,
		ClearDepthColor,
	};

	unsigned int GL_SwitchUsage(GL_Usage usage);

	unsigned int GetType(GL_Type type);
	unsigned int GetTarget(GL_Target type);

	unsigned int GL_SwitchClearCommand(GL_ClearCommand command);
}