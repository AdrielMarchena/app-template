#! /bin/sh

apt install libx11-dev libxcursor-dev libxcursor-dev libxi-dev

git submodule update --init --recursive
./premake5 gmake2
