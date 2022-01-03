project "Game"
	kind "ConsoleApp"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	pchheader "pch.h"
	pchsource ( "src/pch.cpp" )

	files 
	{
		"src/**.h",
		"src/**.c",
		"src/**.cpp",
	}

	includedirs
	{
		"src/",
		"%{IncludeDirectories.Glfw}",
		"%{IncludeDirectories.Glad}",
		"%{IncludeDirectories.ImGui}",
		"%{IncludeDirectories.stb_image}",
		"%{IncludeDirectories.spdlog}",
		"%{IncludeDirectories.glm}",
		"%{IncludeDirectories.debugbreak}"
	}

	links
	{
		"Glad",
		"ImGui",
		"GLFW",
		"opengl32.lib"
	}

	postbuildcommands
	{
		("{COPYDIR} \"./src/**.h\" \"./include/Game\""),
	}

	--filter { 'files:vendor/stb_image/**.h' }
	--	flags { 'NoPCH' }
	--
	--filter { 'files:vendor/stb_image/**.cpp' }
	--	flags { 'NoPCH' }

	filter "system:Windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"GAME_WINDOWS_BUILD",
			"GAME_STATIC_BUILD"
		}

	filter "system:Unix"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"GAME_UNIX_BUILD",
			"GAME_STATIC_BUILD"
		}

	filter "configurations:Debug"
		defines "GAME_DEBUG"
		symbols "On"

	filter "configurations:Release"
		defines "GAME_RELEASE"
		optimize "On"

	filter "configurations:Dist"
		defines "GAME_DIST"
		optimize "On"

	--filter { "system:windows", "configurations:Debug" }
	--	buildoptions "/MTd"
	--
	--filter { "system:windows", "configurations:Release" }
	--	buildoptions "/MT"
	--
	--filter { "system:windows", "configurations:Dist" }
	--	buildoptions "/MT"