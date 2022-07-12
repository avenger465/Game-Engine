#pragma once
#include "System\Interfaces\IRenderer.h"
namespace Engine
{
	class IState
	{
	public:
		virtual ~IState() {}

		virtual bool InitStates(IRenderer* renderer) = 0;

		virtual void ReleaseStates() = 0;
	};

	IState* SetStates(ERenderingAPI type);
}
