#include "epch.h"
#include "IRenderer.h"
#include "Renderer/DirectX11Renderer/Renderer.h"

namespace Engine
{
	IRenderer* NewRenderer(const ERenderingAPI type)
	{
		if (type == ERenderingAPI::DirectX11)
		{
			return new Renderer();
		}
		else return nullptr;
	}
}
