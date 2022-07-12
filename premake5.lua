include "./vendor/premake/premake_customization/solution_items.lua"
include "Dependencies.lua"

workspace "Engine"
	architecture "x86_64"
	startproject "Editor"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	solution_items
	{
		".editorconfig"
	}

	flags
	{
		"MultiProcessorCompile"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"
	include "vendor/premake"
	include "Engine/vendor/imgui"
group ""

group "Core"
include "Engine"
group ""

group "Tools"
include "Editor"
group ""
