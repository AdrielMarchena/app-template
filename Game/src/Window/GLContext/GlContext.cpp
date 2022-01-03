#include "pch.h"
#include "GlContext.h"
#include "glad/glad.h"
#include "GLFW/glfw3.h"

static int s_MaxFragmentShaderTexUnits = 8;
static bool s_GLADInitialized = false;
GlContext::GlContext(GLFWwindow* window)
	:m_Window(window)
{
	GUI_ASSERT(window, "Window is null");
}
GlContext::~GlContext()
{
	Shutdown();
}
void GlContext::Init()
{
	if (!s_GLADInitialized)
	{
		glfwMakeContextCurrent(m_Window);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		GUI_ASSERT(status, "could not initialize Glad!");
		
		s_GLADInitialized = true;
	}

}
void GlContext::Shutdown()
{
	if (s_GLADInitialized)
	{
	}
}
void GlContext::SwapBuffer()
{
	glfwSwapBuffers(m_Window);
}