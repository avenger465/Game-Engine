#include "epch.h"
#include "IShader.h"
#include "Renderer\DirectX11Renderer\DirectX11Shaders.h"

namespace Engine
{
	IShader* SetShader(ERenderingAPI renderertype)
	{
		if (renderertype == ERenderingAPI::DirectX11) // Returns the DirectX 11 shaders
		{
			return new DirectX11Shader();
		}
		else
		{
			return nullptr;
		}
	}
}
