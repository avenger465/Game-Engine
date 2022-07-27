#pragma once

#include "System\Interfaces\IRenderer.h"

namespace Engine
{
	class OpenGLRenderer : public IRenderer
	{
	public:
		~OpenGLRenderer();

		virtual bool InitRenderer(WindowProperties& WindowProps) override;

		virtual void ShutdownRenderer() override;

		virtual const ERenderingAPI GetRenderingType() override { return ERenderingAPI::DirectX11; }

		virtual const std::string GetRenderingTypeString() override { return "OpenGL"; };

		virtual WindowProperties GetWindowProperties() override { return m_WindowProps; }
	private:
		WindowProperties m_WindowProps;
	};
}
