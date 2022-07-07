project "Editor"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "off"

	targetdir ("%{wks.location}/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("%{wks.location}/bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"src/**.h",
		"src/**.cpp"
	}

	includedirs
	{
		"%{wks.location}/Engine/src",
		"%{wks.location}/Engine/vendor"
	}

	links
	{
		"Engine",
		"winmm.lib"
	}

	filter "system:windows"
		systemversion "latest"

	filter "configurations:Debug"
		defines "E_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "E_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Dist"
		defines "E_DIST"
		runtime "Release"
		optimize "on"
