project "stb_image"
	kind "StaticLib"
	language "C"

	targetdir ("stb_image/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("stb_image/bin-int/" .. outputdir .. "/%{prj.name}")

	files 
	{
		"stb_image/stb_image.cpp"
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
