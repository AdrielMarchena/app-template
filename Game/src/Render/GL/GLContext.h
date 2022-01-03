#pragma once

struct GLFWwindow;
namespace Game
{
class GLContext
{
private:
	GLFWwindow* m_Window;
public:
	GLContext(GLFWwindow* window);
	~GLContext();

	void Init();
	void Shutdown();
	void SwapBuffer();

	static int GetFragmetShaderMaxTextureUnits();
};
}

