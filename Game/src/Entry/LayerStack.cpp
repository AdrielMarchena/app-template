#include "pch.h"
#include "LayerStack.h"

namespace Game
{
	LayerStack::LayerStack()
	{
	}

	LayerStack::~LayerStack()
	{
		DeleteAll();
	}

	void LayerStack::DeleteAll()
	{
		if(m_Layers.empty())
			return;

		for (Layer* layer : m_Layers)
			if(layer)
			{
				layer->OnDetach();
				delete layer;
			}
		m_Layers.clear();
	}

	void LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.insert(m_Layers.begin() + m_LayerInsert, layer);
		m_LayerInsert++;
	}

	void LayerStack::PushOverlay(Layer* overlayer)
	{
		m_Layers.emplace_back(overlayer);
	}

	void LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), layer);
		if (it != m_Layers.end())
		{
			(*it)->OnDetach();
			m_Layers.erase(it);
			m_LayerInsert--;
		}
	}

	void LayerStack::PopOverlay(Layer* overlayer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.end(), overlayer);
		if (it != m_Layers.end())
		{
			(*it)->OnDetach();
			m_Layers.erase(it);
		}
	}
}