#include "epch.h"
#include "IRenderer.h"
#include "Renderer/DirectX11Renderer/DirectX11Renderer.h"
#include "Renderer\OpenGLRenderer\OpenGLRenderer.h"

namespace Engine
{
	IRenderer* NewRenderer(const ERenderingAPI type)
	{
		if (type == ERenderingAPI::DirectX11)
		{
			return new DirectX11Renderer();
		}
		if (type == ERenderingAPI::OpenGL)
		{
			return new OpenGLRenderer();
		}
		else return nullptr;
	}
}
