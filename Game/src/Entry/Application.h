#pragma once

#include "ParseArgs.h"
#include "Layer.h"
#include "LayerStack.h"
#include "ImGuiLayer.h"
#include "Event/Events.h"
#include "Event/ApplicationEvent.h"
#include "Window/WindowGlfw.h"

int main(int argc, char** argv);

namespace Game {

	/**
	*  Your start point. The Application class controlls things like
	*  the window and the Game Loop, and contains the
	*  LayerStack, the idea is that you create a class
	*  that inherit from Application, and Add your customized
	*  Layer's to it.
	*  
	*/
	class Application
	{
	private:
		Generic::ConsoleArgsParser m_ConsoleArgs;
		std::unique_ptr<WindowGlfw> m_Window;
		LayerStack m_LayerStack;
		ImGuiLayer* m_ImGuiLayer;
		bool m_Running = false;
		uint64_t m_FrameCount;
	public:
		Application(int argc, char** argv);
		virtual ~Application();

		/**
		* Start's the game loop, can be called after the constructor.
		*/
		void Run();
		/**
		* Event callback method, constrols all events that come from
		* GLFW window event system and send them to the OnEvent method of the Layers.
		* Keep in mind that each event have a handler variable that, if something
		* set it to true, it stops the event there, this is used to block events from a layer
		* to next one, let's say you have a Layer for a menu and other for your game
		* the Menu layer can block a mouse click, so the game does not get it.
		* The Layers are handled backwards, so the last one get the event first
		* (with the exception of the ImGuiLayer, but this is constrolled by Application class)
		*/
		void OnEvent(Event& e);

		/** 
		* Push a new Layer to LayerStack 
		*/
		void PushLayer(Layer* layer);
		/** 
		* Push a layer as a overlay.
		* overlays always stay at the end of the stack
		*/
		void PushOverlay(Layer* overlay);

		const Generic::ConsoleArgsParser& GetCommandLineArguments() const { return m_ConsoleArgs; }
		
		/**
		* Closes the Application
		*/
		void Close();

		WindowGlfw& GetWindow() { return *m_Window; }
		static Application& Get() { return *m_AppInstance; }
		ImGuiLayer& GetImGuiLayer() { return *m_ImGuiLayer; }

		bool ResizeGlViewPort = true;
	private:
		static Application* m_AppInstance;
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		friend int ::main(int argc, char** argv);
	};

	Application* CreateApplication(int argc, char** argv);
}