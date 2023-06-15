@echo off

mingw32-make config=release

echo:
echo Launching...
.\bin\Release\cairo-sdl2-software-rendering.exe

echo:
echo Press enter to exit
set /p input=
exit