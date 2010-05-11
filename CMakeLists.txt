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

set(VACA_SOURCES
    src/Anchor.cpp 
    src/AnchorLayout.cpp
    src/Application.cpp 
    src/BandedDockArea.cpp 
    src/BasicDockArea.cpp
    src/Bix.cpp 
    src/BoxConstraint.cpp 
    src/BoxLayout.cpp 
    src/Brush.cpp
    src/Button.cpp 
    src/ButtonBase.cpp 
    src/CancelableEvent.cpp
    src/CheckBox.cpp 
    src/ChildEvent.cpp 
    src/ClientLayout.cpp
    src/Clipboard.cpp 
    src/CloseEvent.cpp 
    src/Color.cpp 
    src/ColorDialog.cpp
    src/ComboBox.cpp 
    src/Command.cpp 
    src/CommandEvent.cpp
    src/CommonDialog.cpp 
    src/Component.cpp 
    src/ConditionVariable.cpp 
    src/Constraint.cpp 
    src/ConsumableEvent.cpp
    src/Cursor.cpp 
    src/CustomButton.cpp 
    src/CustomLabel.cpp 
    src/Debug.cpp
    src/Dialog.cpp 
    src/DockArea.cpp 
    src/DockBar.cpp 
    src/DockFrame.cpp
    src/DropFilesEvent.cpp 
    src/Event.cpp 
    src/Exception.cpp
    src/FileDialog.cpp 
    src/FindFiles.cpp 
    src/FindTextDialog.cpp
    src/FocusEvent.cpp 
    src/Font.cpp 
    src/FontDialog.cpp 
    src/Frame.cpp
    src/Graphics.cpp 
    src/GraphicsPath.cpp 
    src/GroupBox.cpp
    src/HttpRequest.cpp 
    src/Icon.cpp 
    src/Image.cpp 
    src/ImageList.cpp
    src/KeyEvent.cpp 
    src/Keys.cpp 
    src/Label.cpp 
    src/Layout.cpp
    src/LayoutEvent.cpp 
    src/LinkLabel.cpp 
    src/ListBox.cpp
    src/ListColumn.cpp 
    src/ListItem.cpp 
    src/ListView.cpp 
    src/Mdi.cpp
    src/Menu.cpp 
    src/MenuItemEvent.cpp 
    src/Message.cpp 
    src/MouseEvent.cpp
    src/MsgBox.cpp 
    src/Mutex.cpp 
    src/PaintEvent.cpp 
    src/Pen.cpp
    src/Point.cpp 
    src/PreferredSizeEvent.cpp 
    src/ProgressBar.cpp
    src/Property.cpp 
    src/RadioButton.cpp 
    src/ReBar.cpp 
    src/Rect.cpp
    src/Referenceable.cpp 
    src/Region.cpp 
    src/ResizeEvent.cpp
    src/ResourceId.cpp 
    src/RichEdit.cpp 
    src/Scintilla.cpp
    src/ScrollEvent.cpp 
    src/ScrollInfo.cpp 
    src/ScrollableWidget.cpp
    src/Separator.cpp 
    src/SetCursorEvent.cpp 
    src/Size.cpp 
    src/Slider.cpp
    src/SpinButton.cpp 
    src/Spinner.cpp 
    src/SplitBar.cpp 
    src/StatusBar.cpp
    src/String.cpp 
    src/Style.cpp 
    src/Styles.cpp 
    src/System.cpp 
    src/Tab.cpp
    src/TextEdit.cpp 
    src/Thread.cpp 
    src/TimePoint.cpp 
    src/Timer.cpp
    src/ToggleButton.cpp 
    src/ToolBar.cpp 
    src/TreeNode.cpp 
    src/TreeView.cpp
    src/TreeViewEvent.cpp 
    src/Vaca.cpp 
    src/Widget.cpp 
    src/WidgetClass.cpp)

if(VACA_WINDOWS)
  set(VACA_SOURCES ${VACA_SOURCES} src/win32/win32.cpp)
endif(VACA_WINDOWS)

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
