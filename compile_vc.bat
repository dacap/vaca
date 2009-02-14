@echo off
if "%VCINSTALLDIR%" == "" call "vsvars32.bat"

gnumake\make.exe -f Makefile.vc.gnumake %1 %2 %3 %4 %5 %6 %7 %8 %9
goto :eof
