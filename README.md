# app-template

Template for c++ application using glfw and imgui.
There is some code unrelated to GUI things, 
but you can just delete then and use the template

## Clone

You need to clone the repo recursive

`git clone --recursive git@github.com:AdrielMarchena/app-template.git`

or

`git clone --recursive https://github.com/AdrielMarchena/app-template.git`

To make sure that all submodules are good to go

`git submodule update --init --recursive`

## Build

### Linux (tested on ubuntu)

First you need to have a c++ compiler and `make` installed </br>
i think `apt install build-essential` is enought

You may need to install some things in your machine related to 
'#include <X11/Xcursor/Xcursor.h>' on glfw

i'am not sure what packages you need to install, but this one you must have

`apt install libx11-dev`

To build, first run `./premake5 gmake2` to generate Makefiles
then run `make` to build all dependences and the project it self.

Inside `./Game/bin/` you will find more folders, there names depends
on the configuration of the build. Example: "Debug-linux-x86_64/Game"
and inside this final Game folder, you can find your executable

Since the assets for the project are inside "./Game/assets" place the executable in "./Game"
to run it properly

### Windows

After the cloning step you will need to download premake5 for windows,
the one on the repo is the linux binary, you can download it here: https://premake.github.io/download

Place the "premake5.exe" on the project folder, and run `premake.exe vs2019`
you can change "vs2019" to any other version premake5 supports. This will generate
a visual studio solution, open it and you are good to go.

## TODO

Add a post-build command to copy the bynary to the ./Game folder

## Dependencies

- [GLFW](https://github.com/glfw/glfw) - Windowing
- [GLAD](https://glad.dav1d.de/) - OpenGL Loading Library
- [spdlog](https://github.com/gabime/spdlog) - Log system
- [stb_image](https://github.com/nothings/stb) - load images
- [lodepng](https://github.com/lvandeve/lodepng) - load png images
- [debugbreak](https://github.com/scottt/debugbreak) - provides a debugbreak compatible with the used compiler
- [glm](https://github.com/g-truc/glm)- Vectors, Matrixs and Math
- [ImGui](https://github.com/ocornut/imgui) - for GUI

### Premake

https://premake.github.io/docs/What-Is-Premake

## Contribute

Clone -> create new branch for your changes -> open a PR to 'dev' branch.
