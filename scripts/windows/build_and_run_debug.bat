@echo off

mingw32-make config=debug

echo:
echo Launching...
.\bin\Debug\cairo-sdl2-software-rendering.exe

echo:
echo Press enter to exit
set /p input=
exit