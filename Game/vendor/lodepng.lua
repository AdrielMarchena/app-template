project "lodepng"
	kind "StaticLib"
	language "C++"

	targetdir ("lodepng/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("lodepng/bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"lodepng/lodepng.h",
		"lodepng/lodepng.cpp"
	}

	filter "system:linux"
		pic "On"

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
