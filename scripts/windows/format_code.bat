@echo off

echo Formatting ...

clang-format -i -style=file include/*.h *.c src/*.c

echo Done.

exit

