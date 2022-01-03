#pragma once

#include "Layer.h"

namespace Game
{
	class ImGuiLayer : public Layer
	{
	private:
		bool m_Blockevents = false;
		int m_FontSize = 18;
	public:
		ImGuiLayer(const std::string& name = "ImGuiLayer");

		virtual void OnAttach() override;
		virtual void OnImGuiRender() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& e) override;

		void BlockEvents(bool block) { m_Blockevents = block; }

		void ImGuiInitFrame();
		void ImGuiEndFrame();
	};
}

