# Vaca - Visual Application Components Abstraction
# Copyright (c) 2022 David Capello
# Copyright (c) 2005-2010 David Capello, Jie Zhang
#
# This file is distributed under the terms of the MIT license,
# please read LICENSE.txt for more information.

cmake_minimum_required(VERSION 3.11)

project(vaca)

########################################
# Options

option(SHARED "Build shared libraries" on)
option(THEMES "Build examples using WinXP themes" on)

set(VACA_PLATFORM "Windows" CACHE STRING
  "Vaca as Win32 API wrapper or as Cocoa API wrapper")

# TODO macOS port is not supported, and not planned, but it might be
#      fun to implement in the future
set_property(CACHE VACA_PLATFORM PROPERTY STRINGS
  "Windows" "macOS")

if(VACA_PLATFORM STREQUAL "Windows")
  set(VACA_WINDOWS 1)
  set(vaca_platform_def "-DVACA_WINDOWS")
elseif(VACA_PLATFORM STREQUAL "macOS")
  set(VACA_MACOS 1)
  set(vaca_platform_def "-DVACA_MACOS")
endif()

set(BUILD_SHARED_LIBS ${SHARED})

########################################
# Directories

include_directories(${CMAKE_SOURCE_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

########################################
# Main target: Vaca Library

set(VACA_SOURCES
    vaca/Anchor.cpp
    vaca/AnchorLayout.cpp
    vaca/Application.cpp
    vaca/BandedDockArea.cpp
    vaca/BasicDockArea.cpp
    vaca/Bix.cpp
    vaca/BoxConstraint.cpp
    vaca/BoxLayout.cpp
    vaca/Brush.cpp
    vaca/Button.cpp
    vaca/ButtonBase.cpp
    vaca/CancelableEvent.cpp
    vaca/CheckBox.cpp
    vaca/ClientLayout.cpp
    vaca/Clipboard.cpp
    vaca/CloseEvent.cpp
    vaca/Color.cpp
    vaca/ColorDialog.cpp
    vaca/ComboBox.cpp
    vaca/Command.cpp
    vaca/CommandEvent.cpp
    vaca/CommonDialog.cpp
    vaca/Component.cpp
    vaca/ConditionVariable.cpp
    vaca/Constraint.cpp
    vaca/ConsumableEvent.cpp
    vaca/Cursor.cpp
    vaca/CustomButton.cpp
    vaca/CustomLabel.cpp
    vaca/Debug.cpp
    vaca/Dialog.cpp
    vaca/DockArea.cpp
    vaca/DockBar.cpp
    vaca/DockFrame.cpp
    vaca/DropFilesEvent.cpp
    vaca/Event.cpp
    vaca/Exception.cpp
    vaca/FileDialog.cpp
    vaca/FindFiles.cpp
    vaca/FindTextDialog.cpp
    vaca/FocusEvent.cpp
    vaca/Font.cpp
    vaca/FontDialog.cpp
    vaca/Frame.cpp
    vaca/Graphics.cpp
    vaca/GraphicsPath.cpp
    vaca/GroupBox.cpp
    vaca/HttpRequest.cpp
    vaca/Icon.cpp
    vaca/Image.cpp
    vaca/ImageList.cpp
    vaca/KeyEvent.cpp
    vaca/Keys.cpp
    vaca/Label.cpp
    vaca/Layout.cpp
    vaca/LayoutEvent.cpp
    vaca/LinkLabel.cpp
    vaca/ListBox.cpp
    vaca/ListColumn.cpp
    vaca/ListItem.cpp
    vaca/ListView.cpp
    vaca/Mdi.cpp
    vaca/Menu.cpp
    vaca/MenuItemEvent.cpp
    vaca/Message.cpp
    vaca/MouseEvent.cpp
    vaca/MsgBox.cpp
    vaca/Mutex.cpp
    vaca/PaintEvent.cpp
    vaca/Pen.cpp
    vaca/Point.cpp
    vaca/PreferredSizeEvent.cpp
    vaca/ProgressBar.cpp
    vaca/Property.cpp
    vaca/RadioButton.cpp
    vaca/ReBar.cpp
    vaca/Rect.cpp
    vaca/Referenceable.cpp
    vaca/Region.cpp
    vaca/ResizeEvent.cpp
    vaca/ResourceId.cpp
    vaca/RichEdit.cpp
    vaca/Scintilla.cpp
    vaca/ScrollEvent.cpp
    vaca/ScrollInfo.cpp
    vaca/ScrollableWidget.cpp
    vaca/Separator.cpp
    vaca/SetCursorEvent.cpp
    vaca/Size.cpp
    vaca/Slider.cpp
    vaca/SpinButton.cpp
    vaca/Spinner.cpp
    vaca/SplitBar.cpp
    vaca/StatusBar.cpp
    vaca/String.cpp
    vaca/System.cpp
    vaca/Tab.cpp
    vaca/TextEdit.cpp
    vaca/Thread.cpp
    vaca/TimePoint.cpp
    vaca/Timer.cpp
    vaca/ToggleButton.cpp
    vaca/ToolBar.cpp
    vaca/TreeNode.cpp
    vaca/TreeView.cpp
    vaca/TreeViewEvent.cpp
    vaca/Vaca.cpp
    vaca/Widget.cpp
    vaca/WidgetClass.cpp)

if(VACA_WINDOWS)
  set(VACA_SOURCES ${VACA_SOURCES} vaca/win32/win32.cpp)
endif(VACA_WINDOWS)

add_library(vaca ${VACA_SOURCES})

######################################################################
# Post-build commands to run examples and tests

add_custom_command(TARGET vaca
  POST_BUILD
  COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/scintilla/SciLexer.dll ${CMAKE_CURRENT_BINARY_DIR}/examples
  COMMENT "Copying SciLexer DLL to examples directory")

if(BUILD_SHARED_LIBS)
  add_custom_command(
    TARGET vaca
    BYPRODUCTS
      ${CMAKE_CURRENT_BINARY_DIR}/examples/vaca.dll
      ${CMAKE_CURRENT_BINARY_DIR}/tests/vaca.dll
      ${CMAKE_CURRENT_BINARY_DIR}/tests/gtest.dll
      ${CMAKE_CURRENT_BINARY_DIR}/tests/gtest_main.dll
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vaca> ${CMAKE_CURRENT_BINARY_DIR}/examples
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vaca> ${CMAKE_CURRENT_BINARY_DIR}/tests
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gtest> ${CMAKE_CURRENT_BINARY_DIR}/tests
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:gtest_main> ${CMAKE_CURRENT_BINARY_DIR}/tests
    COMMENT "Copying dlls to examples and tests directories")
endif(BUILD_SHARED_LIBS)

########################################
# Flags

set(win32_flags "-DWINVER=0x0500 -D_WIN32_IE=0x0500 -D_WIN32_WINNT=0x0500")
set(unicode_flags "-DUNICODE -D_UNICODE")
if(NOT BUILD_SHARED_LIBS)
  set(static_flags "-DVACA_STATIC")
endif(NOT BUILD_SHARED_LIBS)

set(common_flags "${win32_flags} ${unicode_flags} ${static_flags}")

set_target_properties(vaca PROPERTIES
  COMPILE_FLAGS "-DVACA_SRC ${common_flags} ${vaca_platform_def}")

target_link_libraries(vaca
  User32 Shell32 ComCtl32 ComDlg32 Gdi32 Msimg32
  WinMM AdvAPI32 Ole32 ShLwApi Vfw32 WinInet)

########################################
# Installation commands

FILE(GLOB vaca_h_files "${CMAKE_CURRENT_SOURCE_DIR}/vaca/*.h")
INSTALL(FILES ${vaca_h_files} DESTINATION include/vaca)

INSTALL(TARGETS vaca
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

########################################
# Examples

add_subdirectory(examples)

########################################
# Tests

enable_testing()
add_subdirectory(tests)
