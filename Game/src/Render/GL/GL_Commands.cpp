#include "pch.h"
#include "Gl_Commands.h"

#include "glad/glad.h"
#include "gl_error_macro_db.h"

namespace Game
{
	void GLCommands::GL_DrawElementsCall(GL_Target target, int32_t size, GL_Type type, const void* indices)
	{
		GLCall(glDrawElements(GetTarget(target), size, GetType(type), indices));
	}
	void GLCommands::GL_DrawArrayCall(GL_Target mode, int32_t first, int32_t count)
	{
		GLCall(glDrawArrays(GetTarget(mode), first, count));
	}
	void GLCommands::GL_Clear(GL_ClearCommand command)
	{
		GLCall(glClear(GL_SwitchClearCommand(command)));
	}
}