#pragma once
#include <string>
#include "System/Interfaces/IRenderer.h"

namespace Engine
{
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer() = default;

		virtual void OnAttach(IRenderer* renderer) {}
		virtual void OnDetach() {}
		virtual void OnUpdate(float frameTime) {}
		virtual void onImGuiRender() {}

		const std::string& GetName() const { return m_Name; }

	protected:
		std::string m_Name;
	};
}
