#pragma once

#include <string>
#include <Windows.h>

namespace Engine
{

	enum class ERenderingType
	{
		None = 0,
		DirectX11
	};

	struct WindowProperties
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		ERenderingType RenderType;
		HWND Hwnd = NULL;

		WindowProperties(const std::string& title = "Engine",
			uint32_t width = 1600,
			uint32_t height = 900,
			ERenderingType renderType = ERenderingType::DirectX11)
			: Title(title), Width(width), Height(height), RenderType(renderType)
		{
		}
	};
}
