IncludeDir = {}
IncludeDir["ImGui"]		    = "%{wks.location}/Engine/vendor/imgui"
IncludeDir["ImGuiBackends"] = "%{wks.location}/Engine/vendor/imgui/backends"
IncludeDir["Assimp"]		= "%{wks.location}/Engine/vendor/assimp/include"
IncludeDir["DirectX"]		= "%{wks.location}/Engine/vendor/DirectXTK"
IncludeDir["GLFW"]		= "%{wks.location}/Engine/vendor/GLFW/include"
IncludeDir["Glad"]		= "%{wks.location}/Engine/vendor/Glad/include"

LibDir = {}
LibDir["assimp"]	= "%{wks.location}/Engine/vendor/assimp/lib/xSixtyFour"
LibDir["DirectXTK"] = "%{wks.location}/Engine/vendor/DirectXTK/DirectX-%{cfg.buildcfg}"
