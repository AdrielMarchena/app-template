project "ImGui"
	kind "StaticLib"
	language "C++"

	targetdir ("imgui/bin/" .. outputdir .. "/%{prj.name}")
	objdir ("imgui/bin-int/" .. outputdir .. "/%{prj.name}")

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
		"imgui/**.h",
		"imgui/**.cpp"
	}

	removefiles
	{
		"imgui/backends/imgui_impl_sdlrenderer.cpp",
		"imgui/backends/imgui_impl_sdl.cpp",
		"imgui/backends/imgui_impl_vulkan.h",
		"imgui/backends/imgui_impl_vulkan.cpp",
		"imgui/backends/imgui_impl_marmalade.h",
		"imgui/backends/imgui_impl_marmalade.cpp",
		"imgui/backends/imgui_impl_allegro5.cpp",
		"imgui/backends/imgui_impl_allegro5.h",
		"imgui/backends/imgui_impl_android.h",
		"imgui/backends/imgui_impl_android.cpp",
		"imgui/backends/imgui_impl_wgpu.h",
		"imgui/backends/imgui_impl_wgpu.cpp",
		"imgui/backends/imgui_impl_glut.cpp",
		"imgui/misc/freetype/imgui_freetype.cpp",
		"imgui/misc/freetype/imgui_freetype.h",
		"imgui/backends/imgui_impl_dx9.cpp",
		"imgui/backends/imgui_impl_dx9.h",
		"imgui/backends/imgui_impl_dx10.cpp",
		"imgui/backends/imgui_impl_dx10.h",
		"imgui/backends/imgui_impl_dx11.cpp",
		"imgui/backends/imgui_impl_dx11.h",
		"imgui/backends/imgui_impl_dx12.cpp",
		"imgui/backends/imgui_impl_dx12.h",
		"imgui/main.cpp",
		"imgui/examples/**.cpp",
		"imgui/examples/**.h",
		"imgui/examples/**.hpp",

		"imgui/backends/imgui_impl_win32.cpp",
	}

	includedirs
	{
		"imgui/include/GLFW",
		"imgui/include",
		"imgui/src",
		"glfw/include",
		"imgui/"
	}

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
