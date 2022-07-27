#include "epch.h"
#include "IWindow.h"
#include "Platforms\WindowsPlatform.h"
#include "Platforms\OpenGLPlatform.h"

namespace Engine
{
	IWindow* NewWindow(WindowProperties& props, IRenderer* renderer)
	{
		if (props.RenderType == ERenderingAPI::DirectX11)
		{
			return new Window(props, renderer);
		}
		if (props.RenderType == ERenderingAPI::OpenGL)
		{
			return new OpenGLWindow(props);
		}
		else return nullptr;
	}
}
