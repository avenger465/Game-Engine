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

		"vendor/imgui/backends/imgui_impl_glfw.h",
		"vendor/imgui/backends/imgui_impl_glfw.cpp",
		"vendor/imgui/backends/imgui_impl_opengl3.h",
		"vendor/imgui/backends/imgui_impl_opengl3.cpp",
		"Src/Shaders/Common.hlsli"
	}

	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"src",
		"vendor/spdlog/include",
		"%{IncludeDir.Assimp}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.ImGuiBackends}",
		"%{IncludeDir.DirectX}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.GLFW}"
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
		"winmm.lib",
		"xinput.lib",
		"GLFW",
		"Glad",
		"opengl32.lib"
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
		defines{"E_PLATFORM_WINDOWS"}

	filter "configurations:Debug"
		defines "E_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "E_Release"
		runtime "Release"
		optimize "on"	

	filter "configurations:Dist"
		defines "E_DIST"
		runtime "Release"
		optimize "on"
