#pragma once

#include <iostream>
#include "debugbreak.h"
#define GUI_ASSERT(expression,message) if(!(expression)) { std::cout << message << std::endl; debug_break(); }

struct GLFWwindow;

class GlContext
{
private:
	GLFWwindow* m_Window;
public:
	GlContext(GLFWwindow* window);
	~GlContext();

	void Init();
	void Shutdown();
	void SwapBuffer();

};

