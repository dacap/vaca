@echo off

REM Main program
set ERRORCODE=1
set RETURN=r1 & goto :checkgxx
:r1
if "%GXX_EXISTS%" == "0" (
  set RETURN=r2 & goto :defmingw
  :r2
  set RETURN=r3 & goto :checkgxx
  :r3
  if "%GXX_EXISTS%" == "0" (
    set RETURN=done & goto :errmingw
  )
)
set RETURN=done & goto :compile
:done
set GXX_EXISTS=
set RETURN=
exit /b %ERRORCODE%

REM compile() subroutine
:compile
gnumake\make.exe -f Makefile.mingw %1 %2 %3 %4 %5 %6 %7 %8 %9
set ERRORCODE=%errorlevel%
goto %RETURN%

REM checkgxx() subroutine
:checkgxx
g++ -dumpversion 1>nul 2>nul
if %errorlevel% == 0 (set GXX_EXISTS=1) else (set GXX_EXISTS=0)
goto %RETURN%

REM defmingw() subroutine
:defmingw
if exist "C:\msys\1.0\bin" set PATH=C:\msys\1.0\bin;%PATH%
if exist "C:\MinGW\bin" set PATH=C:\MinGW\bin;%PATH%
if exist "C:\MinGW" set MINGW_ROOT=C:/MinGW
goto %RETURN%

REM errmingw() subroutine
:errmingw
echo You have to set the location of MinGW\bin in your PATH environment variable.
echo I tried with C:\MinGW\bin but it does not exist.
pause
goto %RETURN%
