#pragma once

#include <memory>
#include "System\Interfaces\IRenderer.h"
#include "Common\EngineProperties.h"


namespace Engine
{
	class IWindow
	{
	public:
		virtual ~IWindow() {}
		virtual void Update(IRenderer* renderer) = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;

		virtual WindowProperties GetWindowProperties() const = 0;
		static IWindow* Create(WindowProperties& props, IRenderer* renderer);
	};
}
