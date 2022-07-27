#pragma once
#include "Common\EngineProperties.h"

namespace Engine
{
	class IRenderer
	{
	public:
		virtual bool InitRenderer(WindowProperties& WindowProps) = 0;

		virtual void ShutdownRenderer() = 0;

		virtual const ERenderingAPI GetRenderingType() = 0;

		virtual const std::string GetRenderingTypeString() = 0;

		virtual WindowProperties GetWindowProperties() = 0;
	};

	IRenderer* NewRenderer(const ERenderingAPI type);
}
