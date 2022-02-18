# app-template

Well, it was only a template, but i did unnecessary thing in this project

## Clone

If you are in linux ubuntu, do

`git clone --recursive link_to_this_repo`

and to make sure that all submodules are good to go

`git submodule update --init --recursive`

The Makefiles are in the repo already, but you can run

`./premake5 gmake2`

to be sure

for windows, i still need to fix some premake5 scripts and test on a windows machine

## Build

First you need to have a c++ compiler and make installed
i think `apt install build-essential` is enought

You may need to install some things in your machine related to 
'#include <X11/Xcursor/Xcursor.h>' on glfw

i'am not sure what packages you need to install, but this you must have

`apt install libx11-dev`

then, you can run `make` to build

to run the app you will need to place the binary on the ./Game/ folder, because the assets are there

## TODO

Fix premake5 scripts to generate valid output for Windows (mainly glfw and imgui .lua)

## Dependencies

Are listed on the Dependencies.lua

### Premake

https://premake.github.io/docs/What-Is-Premake
