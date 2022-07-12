#pragma once

#include "tinyxml2.h"
#include "Common\EngineProperties.h"

namespace Engine
{
	class ParseApplicationSettings
	{
	public:
		ParseApplicationSettings() {}
		WindowProperties ReadWindowSettings(std::string filename);

	private:
		ERenderingAPI GetRenderingAPI(std::string type);

		WindowProperties m_WindowProps;
	};
}
