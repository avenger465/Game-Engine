#include "epch.h"
#include "IState.h"
#include "Renderer\DirectX11Renderer\DirectX11States.h"

namespace Engine
{
	IState* SetStates(ERenderingAPI type)
	{
		if (type == ERenderingAPI::DirectX11) // Returns the DirectX 11 States
		{
			return new DirectX11States();
		}
		else
		{
			return nullptr;
		}
	}
}
