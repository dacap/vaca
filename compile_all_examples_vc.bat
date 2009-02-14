@echo off

echo Compiling Vaca Examples with MSVC...
call compile_vc.bat examples "RELEASE=1" "STATIC_LIBC=1"
