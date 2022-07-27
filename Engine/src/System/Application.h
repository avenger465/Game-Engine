#pragma once

#include "Interfaces\IWindow.h"
#include "Interfaces\IRenderer.h"

namespace Engine
{
	class Application
	{
	public:
		Application();
		~Application();

		void run();

	private:
		//std::unique_ptr<IWindow> m_Window;
		IWindow* m_Window;
		IRenderer* m_Renderer = nullptr;

	};

	Application* CreateApplication();
}
