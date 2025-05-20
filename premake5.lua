workspace "JEngine"
	architecture "x64"

	startproject "Sandbox"

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

project "JEngine"
	location "JEngine"
	kind "SharedLib"
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
		"%{prj.name}/vendor/spdlog/include"
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