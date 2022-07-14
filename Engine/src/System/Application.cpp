#include "epch.h"
#include "Application.h"
#include "Data/ParseApplicationSettings.h"


namespace Engine
{
	Application::Application()
	{
		WindowProperties WindowProps;

		ParseApplicationSettings ApplicationSettings;

		WindowProps = ApplicationSettings.ReadWindowSettings("data/WindowSettings.xml");
		//WindowProps = ApplicationSettings.ReadWindowSettings("data/FullscreenSettings.xml");

		m_Renderer = NewRenderer(WindowProps.RenderType);
		if (m_Renderer != nullptr)
		{
			m_Window = std::unique_ptr<IWindow>(IWindow::Create(WindowProps, m_Renderer));
			if (!m_Window)
			{

			}

			if (!m_Renderer->InitRenderer(WindowProps))
			{
				std::cout << "Error with renderer" << std::endl;
			}
		}
	}

	Application::~Application()
	{
		m_Renderer->ShutdownRenderer();
		delete m_Renderer;
	}

	void Application::run()
	{
		m_Window->Update(m_Renderer);
	}
}
