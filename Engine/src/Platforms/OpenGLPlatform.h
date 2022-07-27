#pragma once

#include "System\Interfaces\IWindow.h"
#include "System\Interfaces\IRenderer.h"

#include<glad/glad.h>
#include <GLFW/glfw3.h>

namespace Engine
{
	class OpenGLWindow : public IWindow
	{
	public:
		OpenGLWindow(WindowProperties& props);

		virtual void Update(IRenderer* renderer) override;
		virtual unsigned int GetWidth() const override { return m_WindowProps.Width; }
		virtual unsigned int GetHeight() const override { return m_WindowProps.Height; }
		virtual WindowProperties GetWindowProperties() const override { return m_WindowProps; }


	private:
		bool Init();
		bool Run();

	private:
		WindowProperties m_WindowProps;

		GLFWwindow* m_Window;
	};
}
