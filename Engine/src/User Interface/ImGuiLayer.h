#pragma once
#include "Layer.h"
#include "Common\EngineProperties.h"

namespace Engine
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		virtual void OnAttach(IRenderer* renderer) override;
		virtual void OnDetach() override;
		
		void Begin();
		void End();

		void SetDarkThemeColours();

	private:
		WindowProperties m_WindowProps;


	};
}
