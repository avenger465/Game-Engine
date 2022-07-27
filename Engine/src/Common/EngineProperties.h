#pragma once

#include <string>
#include <Windows.h>

namespace Engine
{

	typedef unsigned int TInt32;

	typedef TInt32 EntityUID;
	const EntityUID SystemUID = 0xffffffff;

	enum class ERenderingAPI
	{
		None = 0,
		DirectX11,
		OpenGL
	};

	struct WindowProperties
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;

		ERenderingAPI RenderType;
		HWND Hwnd = NULL;

		WindowProperties(const std::string& title = "Engine",
			uint32_t width = 1600,
			uint32_t height = 900,
			ERenderingAPI renderType = ERenderingAPI::DirectX11)
			: Title(title), Width(width), Height(height), RenderType(renderType)
		{
		}
	};
}
