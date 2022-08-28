#include "LayerStack.h"
namespace Engine
{
	Engine::LayerStack::~LayerStack()
	{
		for (Layer* layer : m_Layers)
		{
			layer->OnDetach();
			delete layer;
		}
	}

	void Engine::LayerStack::PushLayer(Layer* layer)
	{
		m_Layers.emplace(m_Layers.begin() + m_InsertIndex, layer);
		++m_InsertIndex;
	}

	void LayerStack::PushOverlay(Layer* overlay)
	{
		m_Layers.emplace_back(overlay);
	}

	void Engine::LayerStack::PopLayer(Layer* layer)
	{
		auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_InsertIndex, layer);
		if (it != m_Layers.begin() + m_InsertIndex)
		{
			layer->OnDetach();
			m_Layers.erase(it);
			m_InsertIndex--;
		}
	}
	void LayerStack::PopOverlay(Layer* overlay)
	{
	}
}
