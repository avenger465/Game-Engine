#include "epch.h"
#include "IRenderer.h"
#include "Renderer\Renderer.h"

namespace Engine
{
	IRenderer* NewRenderer(const ERenderingType type)
	{
		if (type == ERenderingType::DirectX11)
		{
			return new Renderer();
		}
		else return nullptr;
	}
}
