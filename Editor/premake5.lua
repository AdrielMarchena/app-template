project "Editor"
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
		"%{IncludeDirectories.ImGui}",
		"%{IncludeDirectories.stb_image}",
		"%{IncludeDirectories.spdlog}",
		"%{IncludeDirectories.glm}",
		"%{IncludeDirectories.debugbreak}",
		"%{IncludeDirectories.entt}",
		"%{IncludeDirectories.Game}"
	}
		
	filter {}

	links
	{
		"Game",
		"GLFW",
		"stb_image",
		"Glad",
		"ImGui",
		"lodepng",
		"Box2D"
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
			"EDITOR_WINDOWS_BUILD",
			"EDITOR_STATIC_BUILD",
			"GAME_WINDOWS_BUILD"
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
			"EDITOR_LINUX_BUILD",
			"EDITOR_STATIC_BUILD",
			"GAME_LINUX_BUILD"
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
		defines { "EDITOR_DEBUG", "EDITOR_PROFILING" }
		symbols "On"

	filter "configurations:Release"
		defines { "EDITOR_RELEASE", "EDITOR_PROFILING" }
		optimize "On"

	filter "configurations:Dist"
		defines "EDITOR_DIST"
		optimize "On"

	--filter { "system:windows", "configurations:Debug" }
	--	buildoptions "/MTd"
	--
	--filter { "system:windows", "configurations:Release" }
	--	buildoptions "/MT"
	--
	--filter { "system:windows", "configurations:Dist" }
	--	buildoptions "/MT"
