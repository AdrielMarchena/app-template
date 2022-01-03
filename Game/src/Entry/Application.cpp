#include "pch.h"
#include "Application.h"

#include "glad/glad.h"
#include "GLFW/glfw3.h"

namespace Game {

	Application* Application::m_AppInstance = nullptr;
	Application::Application(int argc, char** argv)
		:m_ConsoleArgs(argc, argv)
	{
		WindowSpecification specs;

		specs.Title = specs.Title != "" ? specs.Title : "GUI";

		m_AppInstance = this;
		m_Window = std::make_unique<WindowGlfw>(specs);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		m_FrameCount = 0;

		m_ImGuiLayer = new ImGuiLayer(); //Deleted by layerStack
		PushOverlay(m_ImGuiLayer);
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		double deltaTime = 0.0;
		double lastFrame = 0.0;

		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

		m_Running = true;
		while (m_Running)
		{
			//Check Events
			m_Window->OnUpdate();

			// Delta Time and FPS calculations
			double currentTime = glfwGetTime();
			deltaTime = currentTime - lastFrame;
			lastFrame = currentTime;

			glClear(GL_COLOR_BUFFER_BIT);

			//Game Update 
			for (Layer* layer : m_LayerStack)
				layer->OnUpdate(deltaTime);

			m_ImGuiLayer->ImGuiInitFrame();

			for (Layer* layer : m_LayerStack)
				layer->OnImGuiRender();

			m_ImGuiLayer->ImGuiEndFrame();
			m_FrameCount++;
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher disp(e);

		disp.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		//m_ImGuiLayer->OnEvent(e);
		for (auto it = m_LayerStack.end(); it != m_LayerStack.begin();)
		{
			if (e.Handled)
				break;
			(*--it)->OnEvent(e);
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		m_Running = false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Close();
		return true;
	}
}