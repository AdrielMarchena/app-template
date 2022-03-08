#pragma once

#include "Event/Events.h"
#include "Timestamp.h"
#include <string>

namespace Game
{
	class Layer
	{
	protected:
		std::string m_DebugName;
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() { OnDetach(); };

		virtual void OnAttach(){}
		virtual void OnUpdate(Timestamp dt){}
		virtual void OnImGuiRender(){}
		virtual void OnDetach(){}
		virtual void OnEvent(Event& e){}

		const std::string& GetName() const { return m_DebugName; }
	};
}

