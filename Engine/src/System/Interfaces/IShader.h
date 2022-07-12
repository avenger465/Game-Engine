#pragma once
#include "IRenderer.h"

namespace Engine
{
	class IShader
	{
	public:
		virtual ~IShader() {}

		// Initialize the shaders 
		virtual bool InitShaders(IRenderer* renderer) = 0;

		// Release the shaders
		virtual void ReleaseShaders() = 0;
	};

	// Sets the shader depending on the renderer
	IShader* SetShader(ERenderingAPI type);
}
