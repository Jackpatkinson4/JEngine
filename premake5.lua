workspace "JEngine"
	architecture "x64"

	startproject "Sandbox"

	staticruntime "On"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

-- Include directories relative to root folder (solution directory)
IncludeDir = {}
IncludeDir["GLFW"] = "JEngine/vendor/GLFW/include"
IncludeDir["Glad"] = "JEngine/vendor/Glad/include"
IncludeDir["ImGui"] = "JEngine/vendor/imgui"
IncludeDir["glm"] = "JEngine/vendor/glm"

include "JEngine/vendor/GLFW"
include "JEngine/vendor/Glad"
include "JEngine/vendor/imgui"

project "JEngine"
	location "JEngine"
	kind "StaticLib"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	characterset("Unicode")
	buildoptions "/utf-8"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "jepch.h"
	pchsource "JEngine/src/jepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp",
		"%{prj.name}/vendor/glm/**.hpp",
		"%{prj.name}/vendor/glm/**.inl",
	}


	defines
	{
		"_CRT_SECURE_NO_WARNINGS"
	}

	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}",
		"%{IncludeDir.Glad}",
		"%{IncludeDir.ImGui}",
		"%{IncludeDir.glm}"
	}

	links
	{
		"GLFW",
		"Glad",
		"ImGui",
		"opengl32.lib",
		"dwmapi.lib"
	}

	filter "system:windows"
		systemversion "latest"

		defines
		{
			"JE_PLATFORM_WINDOWS",
			"JE_BUILD_DLL",
			"GLFW_INCLUDE_NONE",
		}

	filter "configurations:Debug"
		defines "JE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "JE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Release"
		defines "JE_DIST"
		runtime "Release"
		optimize "on"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
	cppdialect "C++17"
	staticruntime "on"

	characterset("Unicode")
	buildoptions "/utf-8"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}


	includedirs
	{
		"JEngine/vendor/spdlog/include",
		"JEngine/src",
		"JEngine/vendor",
		"%{IncludeDir.glm}"
	}

	links 
	{
		"JEngine"
	}

	filter "system:windows"
		systemversion "latest"

		defines 
		{
			"JE_PLATFORM_WINDOWS",
			"IMGUI_API=__declspec(dllimport)"
		}

	filter "configurations:Debug"
		defines "JE_DEBUG"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		defines "JE_RELEASE"
		runtime "Release"
		optimize "on"

	filter "configurations:Release"
		defines "JE_DIST"
		runtime "Release"
		optimize "on"
