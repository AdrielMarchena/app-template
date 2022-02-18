project "Game"
	kind "ConsoleApp"
	language "C++"

	pchheader "pch.h"
	pchsource ( "src/pch.cpp" )

	files 
	{
		--"vendor/stb_image/stb_image.cpp",
		
		"src/**.h",
		"src/**.c",
		"src/**.cpp",

	}

	includedirs
	{
		"src/",
		--"%{IncludeDirectories.Glfw}",
		"%{IncludeDirectories.Glad}",
		"%{IncludeDirectories.ImGui}",
		"%{IncludeDirectories.stb_image}",
		"%{IncludeDirectories.spdlog}",
		"%{IncludeDirectories.glm}",
		"%{IncludeDirectories.debugbreak}"
	}

	links
	{
		"pthread ",
		"dl",
		"GLFW",
		"stb_image",
		"Glad",
		"ImGui",
		--"opengl32.lib"
		--"glfw3.lib ",
	}

	postbuildcommands
	{
		--("{COPYDIR} \"./src/**.h\" \"./include/Game\""),
	}

	--filter { 'files:vendor/stb_image/**.h' }
	--	flags { 'NoPCH' }
	--
	--filter { 'files:vendor/stb_image/**.cpp' }
	--	flags { 'NoPCH' }

	filter "system:Windows"
		system "windows"
		
		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	filter "system:Unix"
		system "linux"

		targetdir ("bin/" .. outputdir .. "/%{prj.name}")
		objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	filter "system:Windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"GAME_WINDOWS_BUILD",
			"GAME_STATIC_BUILD"
		}

	filter "system:linux"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"GAME_LINUX_BUILD",
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
