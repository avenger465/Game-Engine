project "Engine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "epch.h"
	pchsource "src/epch.cpp"

	files
	{
		"src/**.h",
		"src/**.cpp",
		"vendor/imgui/backends/imgui_impl_dx11.h",
		"vendor/imgui/backends/imgui_impl_dx11.cpp",
		"vendor/imgui/backends/imgui_impl_win32.h",
		"vendor/imgui/backends/imgui_impl_win32.cpp",
		"Src/Shaders/Common.hlsli"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuiBackends}",
		"%{IncludeDir.DirectX}",
		"%{IncludeDir.TinyXML2}"
	}

	libdirs
	{
		"%{LibDir.assimp}",
		"%{LibDir.DirectXTK}"
	}

	links
	{
		"ImGui",
		"d3d11.lib",
		"assimp-vc140-mt.lib",
		"DirectXTK.lib",
		"DirectXTex.lib",
		"d3dcompiler.lib",
		"winmm.lib"
	}

	files("vendor/imgui/backends/imgui_impl_win32.cpp")
	flags("NoPCH")

	files("vendor/imgui/backends/imgui_impl_dx11.cpp")
	flags("NoPCH")

	files("Src/Shaders/*.hlsl")
	shadermodel("5.0")

	local shader_dir = "Src/Shaders/"
	
	filter("files:**.hlsl")
		shaderobjectfileoutput(shader_dir.."%{file.basename}"..".cso")
	
	filter("files:**_ps.hlsl")
		shadertype("Pixel")

	filter("files:**_gs.hlsl")
		shadertype("Geometry")

	filter("files:**_vs.hlsl")
		shadertype("Vertex")
		shaderoptions({"/WX"})

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"DXE_PLATFORM_WINDOWS",
		}

	filter "configurations:Debug"
		defines "E_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "E_Release"
		runtime "Release"
		optimize "on"	
