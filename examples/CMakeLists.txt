# Vaca - Visual Application Components Abstraction
# Copyright (c) 2005-2009 David Capello
# All rights reserved.

function(add_vaca_example name)
  file(GLOB example_sources ${name}/*.cpp)

  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${name}/${name}.rc)
    set(example_resource ${name}/${name}.rc)
  else()
    set(example_resource ${CMAKE_CURRENT_SOURCE_DIR}/Example.rc)
  endif()

  add_executable(${name} WIN32 ${example_sources} ${example_resource})

  set_target_properties(${name} PROPERTIES
    COMPILE_FLAGS "${common_flags}")

  target_link_libraries(${name} Vaca)
  target_link_libraries(${name} ${win32_libs})

  if(THEMES AND MSVC)
    add_custom_command(TARGET ${name}
      POST_BUILD
      COMMAND mt.exe -nologo
		     -manifest ${CMAKE_CURRENT_SOURCE_DIR}/Example.manifest
			       ${CMAKE_CURRENT_BINARY_DIR}/${name}.exe.embed.manifest
		     -outputresource:${CMAKE_CURRENT_BINARY_DIR}/${name}.exe;#1
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMENT "Adding dependecy to themed Common Controls in ${name}.exe")
  endif(THEMES AND MSVC)
endfunction(add_vaca_example)

add_vaca_example(AnchorLayouts)
add_vaca_example(AutoCompletion)
add_vaca_example(Bixes)
add_vaca_example(BouncingBalls)
add_vaca_example(BoxLayouts)
add_vaca_example(Buttons)
add_vaca_example(ColoredButton)
add_vaca_example(ComboBoxes)
add_vaca_example(Commands)
add_vaca_example(DataGrids)
add_vaca_example(DialogResource)
add_vaca_example(EditableListBox)
add_vaca_example(Edits)
add_vaca_example(EyeDropper)
add_vaca_example(FontMetrics)
add_vaca_example(FreeOfLayout)
add_vaca_example(Full)
add_vaca_example(Hashing)
add_vaca_example(HelloWorld)
add_vaca_example(Images)
add_vaca_example(Labels)
add_vaca_example(LikeScript)
add_vaca_example(Maths)
add_vaca_example(MenuResource)
add_vaca_example(MiniExplorer)
add_vaca_example(Minimal)
add_vaca_example(Paths)
add_vaca_example(Primitives)
add_vaca_example(ProgressBars)
add_vaca_example(ReBars)
add_vaca_example(Regions)
add_vaca_example(Scribble)
add_vaca_example(Scrolling)
add_vaca_example(Sliders)
add_vaca_example(Spinners)
add_vaca_example(SplitBars)
add_vaca_example(Sudoku)
add_vaca_example(SystemImageList)
add_vaca_example(Tabs)
add_vaca_example(TextEditor)
add_vaca_example(Threads)
add_vaca_example(Timers)
add_vaca_example(ToolBars)
add_vaca_example(Trees)
add_vaca_example(Undo)
add_vaca_example(WebCam)
