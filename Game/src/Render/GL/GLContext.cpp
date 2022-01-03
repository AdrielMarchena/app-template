#include "pch.h"
#include "GLContext.h"
#include "Log/Log.h"
#include "Core/Core.h"
#include "glad/glad.h"
#include "gl_error_macro_db.h"
#include "GLFW/glfw3.h"

//#define BASE_PRINT_OPENGL_EXTENSIONS

namespace Game
{
	static int s_MaxFragmentShaderTexUnits = 8;
	static bool s_GLADInitialized = false;
	GLContext::GLContext(GLFWwindow* window)
		:m_Window(window)
	{
		GAME_CORE_ASSERT(window, "Window is null");
	}
	GLContext::~GLContext()
	{
		Shutdown();
	}
	void GLContext::Init()
	{
		if (!s_GLADInitialized)
		{
			glfwMakeContextCurrent(m_Window);
			int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
			GAME_CORE_ASSERT(status, "GLAD Initialized!");
			GAME_LOG_INFO("OpenGL Info:");
			GAME_LOG_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
			GAME_LOG_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
			GAME_LOG_INFO("  Version: {0}", glGetString(GL_VERSION));

			int texture_units = 0;
			GLCall(glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &texture_units));
			GAME_LOG_INFO("  GL_MAX_TEXTURE_IMAGE_UNITS: {0}", texture_units);
			if (texture_units > 8)
				s_MaxFragmentShaderTexUnits = texture_units;

			GLCall(glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS, &texture_units));
			GAME_LOG_INFO("  GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS: {0}", texture_units);

			GAME_LOG_INFO("  Shading Version: {0}", glGetString(GL_SHADING_LANGUAGE_VERSION));
			
#ifdef BASE_PRINT_OPENGL_EXTENSIONS
			int count = 0;
			GLCall(glGetIntegerv(GL_NUM_EXTENSIONS, &count));
			BASE_INFO("  Extensions:");
			for (int i = 0; i < count; i++)
			{
				std::string ret;
				GLCall(ret = (const char*)glGetStringi(GL_EXTENSIONS, i));
				BASE_INFO("  {0}: ", ret);
			}
#endif
			s_GLADInitialized = true;
		}

	}
	void GLContext::Shutdown()
	{
		if (s_GLADInitialized)
		{
		}
	}
	void GLContext::SwapBuffer()
	{
		glfwSwapBuffers(m_Window);
	}
	int GLContext::GetFragmetShaderMaxTextureUnits()
	{
		return s_MaxFragmentShaderTexUnits;
	}
}