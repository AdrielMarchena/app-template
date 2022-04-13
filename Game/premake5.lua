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
		"%{IncludeDirectories.debugbreak}",
		"%{IncludeDirectories.lodepng}",
		"%{IncludeDirectories.Box2D}",
		"%{IncludeDirectories.entt}",
		"%{IncludeDirectories.fmod}",
	}
		
	filter {}

	links
	{
		"GLFW",
		"stb_image",
		"Glad",
		"ImGui",
		"lodepng",
		"Box2D"
		--"opengl32.lib"
		--"glfw3.lib ",
	}

	defines
	{
		"GAME_USING_ENTITYID"
	}

	postbuildcommands
	{
		--("{COPYDIR} \"./src/**.h\" \"./include/Game\""),
	}

	filter "system:Windows"
		cppdialect "C++17"
		staticruntime "On"
		systemversion "latest"

		defines
		{
			"GAME_WINDOWS_BUILD",
			"GAME_STATIC_BUILD"
		}

		links
		{
			"fmod_vc"
		}

		filter "architecture:x86"
			libdirs
			{
				"%{wks.location}/Game/vendor/fmodstudioapi20206windows/api/core/lib/x86"
			}

		filter "architecture:x64"
			libdirs
			{
				"%{wks.location}/Game/vendor/fmodstudioapi20206windows/api/core/lib/x64"
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
		
		links
		{
			"pthread",
			"dl",
			"fmod"
		}

		filter "architecture:x86"
			libdirs
			{
				"%{wks.location}/Game/vendor/fmodstudioapi20206linux/api/core/lib/x86"
			}

		filter "architecture:x64"
			libdirs
			{
				"%{wks.location}/Game/vendor/fmodstudioapi20206linux/api/core/lib/x86_64"
			}

	filter "configurations:Debug"
		defines { "GAME_DEBUG", "GAME_PROFILING" }
		symbols "On"

	filter "configurations:Release"
		defines { "GAME_RELEASE", "GAME_PROFILING" }
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
