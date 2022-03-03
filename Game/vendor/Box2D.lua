project "Box2D"
	kind "StaticLib"
	language "C++"
	cppdialect "C++11"
	staticruntime "off"

	targetdir ("Box2D/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("Box2D/bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"Box2D/src/**.h",
		"Box2D/src/**.cpp",
		"Box2D/include/**.h"
	}
	
	includedirs
	{
		"Box2D/include",
		"Box2D/src"
	}

	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		
	filter "configurations:Dist"
		runtime "Release"
		symbols "on"

	filter { "system:windows", "configurations:Debug" }
		buildoptions "/MTd"
	
	filter { "system:windows", "configurations:Release" }
		buildoptions "/MT"
	
	filter { "system:windows", "configurations:Dist" }
		buildoptions "/MT"