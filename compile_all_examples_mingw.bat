@echo off

echo Compiling Vaca Examples with MinGW...
call compile_mingw.bat examples "RELEASE=1" "ADD_COMPILER_VERSION=1"
