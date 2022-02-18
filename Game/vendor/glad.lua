project "Glad"
	kind "StaticLib"
	language "C"
	staticruntime "off"

	targetdir ("glad/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("glad/bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"glad/include/glad/glad.h",
		"glad/include/KHR/khrplatform.h",
		"glad/src/glad.c"
	}

	includedirs
	{
		"glad/include"
	}

	filter "system:linux"
		systemversion "latest"
		staticruntime "On"

	filter "system:windows"
		systemversion "latest"
		staticruntime "On"
		
	filter "configurations:Debug"
		runtime "Debug"
		symbols "on"

	filter "configurations:Release"
		runtime "Release"
		optimize "On"
		
	filter "configurations:Dist"
		runtime "Release"
		symbols "on"
