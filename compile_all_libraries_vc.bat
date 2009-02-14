@echo off

echo Compiling Vaca Release (DLL) version for MSVC...
call compile_vc.bat lib "RELEASE=1" "STATIC_LIBC=1"
if not %errorlevel% == 0 goto :eof

echo Compiling Vaca Release (static) version for MSVC...
call compile_vc.bat lib "RELEASE=1" "STATIC=1" "STATIC_LIBC=1"
if not %errorlevel% == 0 goto :eof

echo Compiling Vaca Debug (DLL) version for MSVC...
call compile_vc.bat lib "DEBUG=1" "STATIC_LIBC=1"
if not %errorlevel% == 0 goto :eof

echo Compiling Vaca Debug (static) version for MSVC...
call compile_vc.bat lib "DEBUG=1" "STATIC=1" "STATIC_LIBC=1"
