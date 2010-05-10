# Vaca - Visual Application Components Abstraction
# Copyright (c) 2005-2010 David Capello, Jie Zhang
# All rights reserved

cmake_minimum_required(VERSION 2.6)

project(Vaca)

########################################
# Options

option(SHARED "Build shared libraries" on)
option(THEMES "Build examples using WinXP themes" on)

set(VACA_PLATFORM "Windows" CACHE STRING
  "Vaca as Win32 API wrapper or Allegro 4.2 wrapper")
set_property(CACHE VACA_PLATFORM PROPERTY STRINGS
  "Windows" "Allegro")

if(VACA_PLATFORM STREQUAL "Windows")
  set(VACA_WINDOWS 1)
  set(vaca_platform_def "-DVACA_WINDOWS")
elseif(VACA_PLATFORM STREQUAL "Allegro")
  set(VACA_ALLEGRO 1)
  set(vaca_platform_def "-DVACA_ALLEGRO")
endif()

set(BUILD_SHARED_LIBS ${SHARED})

########################################
# Directories

include_directories(${CMAKE_SOURCE_DIR}/include
		    ${CMAKE_SOURCE_DIR}/scintilla/include)

set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

########################################
# Main target: Vaca Library

aux_source_directory(src VACA_SOURCES)

add_library(Vaca ${VACA_SOURCES})

######################################################################
# Post-build commands to run examples and tests

add_custom_command(TARGET Vaca
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/scintilla/SciLexer.dll ${CMAKE_CURRENT_BINARY_DIR}/examples
  COMMENT "Copying SciLexer DLL to examples directory")

if(BUILD_SHARED_LIBS)
  get_target_property(vaca_dll_location Vaca LOCATION)
  add_custom_command(TARGET Vaca
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${vaca_dll_location} ${CMAKE_CURRENT_BINARY_DIR}/examples
    COMMAND ${CMAKE_COMMAND} -E copy ${vaca_dll_location} ${CMAKE_CURRENT_BINARY_DIR}/tests
    COMMENT "Copying Vaca DLL to examples and tests directories")
endif(BUILD_SHARED_LIBS)

########################################
# Win32 libraries

set(win32_libs
	User32 Shell32 ComCtl32 ComDlg32 Gdi32 Msimg32
	WinMM AdvAPI32 Ole32 ShLwApi Vfw32 WinInet)

########################################
# Flags

set(win32_flags "-DWINVER=0x0500 -D_WIN32_IE=0x0500 -D_WIN32_WINNT=0x0500")
set(unicode_flags "-DUNICODE -D_UNICODE")
if(NOT BUILD_SHARED_LIBS)
  set(static_flags "-DVACA_STATIC")
endif(NOT BUILD_SHARED_LIBS)

set(common_flags "${win32_flags} ${unicode_flags} ${static_flags}")

set_target_properties(Vaca PROPERTIES
  COMPILE_FLAGS "-DVACA_SRC ${common_flags} ${vaca_platform_def}")

if(BUILD_SHARED_LIBS)
  target_link_libraries(Vaca ${win32_libs})
endif(BUILD_SHARED_LIBS)

########################################
# Installation commands

FILE(GLOB vaca_h_files "${CMAKE_CURRENT_SOURCE_DIR}/include/Vaca/*.h")
INSTALL(FILES ${vaca_h_files} DESTINATION include/Vaca)

INSTALL(TARGETS Vaca
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

########################################
# Examples

add_subdirectory(examples)

########################################
# Tests

add_subdirectory(tests)
