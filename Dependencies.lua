IncludeDir = {}
IncludeDir["ImGui"]		    = "%{wks.location}/Engine/vendor/imgui"
IncludeDir["ImGuiBackends"] = "%{wks.location}/Engine/vendor/imgui/backends"
IncludeDir["Assimp"]		= "%{wks.location}/Engine/vendor/assimp/include"
IncludeDir["DirectX"]		= "%{wks.location}/Engine/vendor/DirectXTK"
IncludeDir["TinyXML2"]		= "%{wks.location}/Engine/vendor/TinyXML2"

LibDir = {}
LibDir["assimp"]	= "%{wks.location}/Engine/vendor/assimp/lib/xSixtyFour"
LibDir["DirectXTK"] = "%{wks.location}/Engine/vendor/DirectXTK/%{cfg.buildcfg}"
