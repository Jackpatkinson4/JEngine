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

include "JEngine/vendor/GLFW"

project "JEngine"
	location "JEngine"
	kind "SharedLib"
	language "C++"
	characterset("Unicode")
	buildoptions "/utf-8"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "jepch.h"
	pchsource "JEngine/src/jepch.cpp"

	files
	{
		"%{prj.name}/src/**.h",
		"%{prj.name}/src/**.cpp"
	}


	includedirs
	{
		"%{prj.name}/src",
		"%{prj.name}/vendor/spdlog/include",
		"%{IncludeDir.GLFW}"
	}

	links
	{
		"GLFW",
		"opengl32.lib",
		"dwmapi.lib"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"JE_PLATFORM_WINDOWS",
			"JE_BUILD_DLL"
		}

		postbuildcommands 
		{
			("{RMDIR} ../bin/" .. outputdir .. "/Sandbox"),
			("{MKDIR} ../bin/" .. outputdir .. "/Sandbox"),
			("{COPYFILE} %{cfg.buildtarget.relpath} ../bin/" .. outputdir .. "/Sandbox")
		}

	filter "configurations:Debug"
		defines "JE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "JE_RELEASE"
		optimize "On"

	filter "configurations:Release"
		defines "JE_DIST"
		optimize "On"

project "Sandbox"
	location "Sandbox"
	kind "ConsoleApp"
	language "C++"
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
		"JEngine/src"
	}

	links 
	{
		"JEngine"
	}

	filter "system:windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines "JE_PLATFORM_WINDOWS"

	filter "configurations:Debug"
		defines "JE_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "JE_RELEASE"
		optimize "On"

	filter "configurations:Release"
		defines "JE_DIST"
		optimize "On"