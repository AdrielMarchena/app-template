project "ImGui"
	kind "StaticLib"
	language "C++"

	targetdir ("bin/" .. outputdir .. "/%{prj.name}")
	objdir ("bin-int/" .. outputdir .. "/%{prj.name}")

	files
	{
		--"imconfig.h",
		--"imgui.h",
		--"imgui.cpp",
		--"imgui_draw.cpp",
		--"imgui_internal.h",
		--"imgui_widgets.cpp",
		--"imstb_rectpack.h",
		--"imstb_textedit.h",
		--"imstb_truetype.h",
		--"imgui_demo.cpp",
		--"backends/imgui_impl_glfw.h",
		--"backends/imgui_impl_glfw.cpp",
		--"backends/imgui_impl_opengl3.h",
		--"backends/imgui_impl_opengl3.cpp"
		"**.h",
		"**.cpp"
	}

	removefiles
	{
		"backends/imgui_impl_sdlrenderer.cpp",
		"backends/imgui_impl_sdl.cpp",
		"backends/imgui_impl_vulkan.h",
		"backends/imgui_impl_vulkan.cpp",
		"backends/imgui_impl_marmalade.h",
		"backends/imgui_impl_marmalade.cpp",
		"backends/imgui_impl_allegro5.cpp",
		"backends/imgui_impl_allegro5.h",
		"backends/imgui_impl_android.h",
		"backends/imgui_impl_android.cpp",
		"backends/imgui_impl_wgpu.h",
		"backends/imgui_impl_wgpu.cpp",
		"backends/imgui_impl_glut.cpp",
		"misc/freetype/imgui_freetype.cpp",
		"misc/freetype/imgui_freetype.h",
		"backends/imgui_impl_dx9.cpp",
		"backends/imgui_impl_dx9.h",
		"backends/imgui_impl_dx10.cpp",
		"backends/imgui_impl_dx10.h",
		"backends/imgui_impl_dx11.cpp",
		"backends/imgui_impl_dx11.h",
		"backends/imgui_impl_dx12.cpp",
		"backends/imgui_impl_dx12.h",
		"main.cpp",
		"examples/**.cpp",
		"examples/**.h",
		"examples/**.hpp",

		"backends/imgui_impl_win32.cpp",
	}

	includedirs
	{
		"include/GLFW",
		"include",
		"src",
		"../glfw/include",
		"./"
	}

	filter "system:Windows"
		system "windows"

	filter "system:Unix"
		system "linux"

	filter "system:windows"
		systemversion "latest"
		cppdialect "C++17"
		staticruntime "On"

	filter "system:linux"
		pic "On"
		systemversion "latest"
		cppdialect "C++17"
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