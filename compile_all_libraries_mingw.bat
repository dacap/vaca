@echo off

echo Compiling Vaca Release (DLL) version for MinGW...
call compile_mingw.bat lib "RELEASE=1" "ADD_COMPILER_VERSION=1"
if not %errorlevel% == 0 goto :eof

echo Compiling Vaca Release (static) version for MinGW...
call compile_mingw.bat lib "RELEASE=1" "STATIC=1" "ADD_COMPILER_VERSION=1"
if not %errorlevel% == 0 goto :eof

echo Compiling Vaca Debug (DLL) version for MinGW...
call compile_mingw.bat lib "DEBUG=1" "ADD_COMPILER_VERSION=1"
if not %errorlevel% == 0 goto :eof

echo Compiling Vaca Debug (static) version for MinGW...
call compile_mingw.bat lib "DEBUG=1" "STATIC=1" "ADD_COMPILER_VERSION=1"
