#pragma once

#include "Layer.h"

#include <vector>
namespace Game
{
	class LayerStack : public Layer
	{
	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsert = 0;
	public:
		LayerStack();
		~LayerStack();

		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlayer);
		void PopLayer(Layer* layer);
		void PopOverlay(Layer* overlayer);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	};
}

