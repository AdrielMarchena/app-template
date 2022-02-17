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
include "Game/vendor/stb_image"
include "Game/vendor/glfw"
include "Game/vendor/imgui"
include "Game/vendor/glad"
group ""

--Projects
include "Game"
--include "Sandbox"
