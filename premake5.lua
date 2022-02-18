include "Dependencies.lua"
workspace "Game"
	startproject "Game"

	platforms
	{
		"x64",
		"x86"
	}

	system
	(
		"Windows",
		"Unix"
	)

	configurations
	{
		"Debug",
		"Release",
		"Dist"
	}

	flags
	{
		"MultiProcessorCompile"
	}

	filter "platforms:x64"
		architecture "x64"
	
	filter "platforms:x86"
		architecture "x86"

	filter "system:Windows"
		system "windows"
		
	filter "system:Unix"
		system "linux"
	
	filter "system:Linux"
		system "linux"
	
	
outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

group "Dependencies"

--Libs
include "Game/vendor/stb_image.lua"
include "Game/vendor/glfw.lua"
include "Game/vendor/imgui.lua"
include "Game/vendor/glad.lua"
include "Game/vendor/lodepng.lua"
group ""

--Projects
include "Game"
--include "Sandbox"
