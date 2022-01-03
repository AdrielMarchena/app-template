#pragma once

#include "GLContext/GlContext.h"
#include <string>
#include <memory>
#include "Event/Events.h"
#include <functional>

struct GLFWmonitor;

namespace Game
{
	struct WindowSpecification
	{
		std::string Title = "GUI";
		int32_t Width = 800;
		int32_t Height = 600;
		bool VSync_On = true;
		bool Fullscreen = false;
		bool Decorated = true;
		bool Resizeble = true;
	};

	class WindowGlfw
	{
	public:
		using EventCallbackFn = std::function<void(Event&)>;
	private:
		GLFWwindow* m_Window;
		WindowSpecification m_Specifications;
		std::unique_ptr<GlContext> m_OpenGLContext;
		bool m_Minimized = false;
		struct WindowData
		{
			std::string Title;
			int32_t Width;
			int32_t Height;
			int32_t Back_Width;
			int32_t Back_Height;
			int32_t XPos = 0;
			int32_t YPos = 0;
			bool VSync_On;
			bool Minimized;
			bool Fullscreen;
			bool TitleBar;
			bool Resizeble;
			EventCallbackFn EventCallback;
		} m_Data;

		int m_MonitorCount;
		GLFWmonitor** m_Monitors;
		int m_CurrentMonitorIndice = 0;
	public:
		WindowGlfw(const WindowSpecification& specs);
		virtual ~WindowGlfw();

		void OnUpdate();

		int32_t GetWidth() const { return m_Data.Width; }
		int32_t GetHeight() const { return m_Data.Height; }

		void SetEventCallback(const EventCallbackFn& callback) { m_Data.EventCallback = callback; }
		void SetVSync(bool enabled);
		bool GetVSync() const;
		bool IsMinimized() const { return m_Data.Minimized; }
		void* GetNative() const { return (void*)m_Window; }

		void SetFullscreen(bool enabled);
		void SetTitleBar(bool enabled);
		void SetResizeble(bool enabled);

		bool glfwIsFullScreen() const;
		bool IsFullscreen() const;
		bool IsTitleBar() const;
		bool IsResizeble() const;
	private:
		void Init(const WindowSpecification& specs);
		void Shutdown();

	};
}



