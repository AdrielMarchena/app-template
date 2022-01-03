#include "pch.h"
#include "ImGuiLayer.h"

#include "Application.h"

#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"
#include "GLFW/glfw3.h"
namespace Game
{
	static bool s_ImGuiInitialized = false;

	ImGuiLayer::ImGuiLayer(const std::string& name)
		:Layer(name)
	{
	}

	void ImGuiLayer::OnAttach()
	{
		if (!s_ImGuiInitialized)
		{
			GLFWwindow* window = (GLFWwindow*)Application::Get().GetWindow().GetNative();

			IMGUI_CHECKVERSION();
			ImGui::CreateContext();
			ImGuiIO& io = ImGui::GetIO(); (void)io;
			io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
			io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
			// Setup Platform/Renderer bindings
			ImGui_ImplGlfw_InitForOpenGL(window, true);
			ImGui_ImplOpenGL3_Init("#version 330");
			// Setup Dear ImGui style
			ImGui::StyleColorsDark();

			io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/fonts/Consola.ttf", m_FontSize);

			s_ImGuiInitialized = true;
		}
	}

	void ImGuiLayer::OnImGuiRender()
	{
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnEvent(Event& e)
	{
		if (m_Blockevents)
		{
			ImGuiIO& io = ImGui::GetIO();
			e.Handled |= e.IsInCategory(EVENT_CATEGORY_MOUSE) & io.WantCaptureMouse;
			e.Handled |= e.IsInCategory(EVENT_CATEGORY_KEYBOARD) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::ImGuiInitFrame()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::ImGuiEndFrame()
	{
		ImGuiIO& io = ImGui::GetIO();
		Application& app = Application::Get();
		io.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backup_context = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backup_context);
		}
	}
}
