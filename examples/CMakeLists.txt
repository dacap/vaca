# Vaca - Visual Application Components Abstraction
# Copyright (c) 2005-2009 David Capello
# All rights reserved.

# Custom rule to compile resources using windres.exe
if(MINGW)
  set(example_resource_rc Example.rc)
  set(example_resource_obj ${CMAKE_CURRENT_BINARY_DIR}/Example.res.obj)
  set(windres_flags -O coff)
  if(THEMES)
    list(APPEND windres_flags -D EXAMPLE_RC_WITH_MANIFEST=1)
  endif()

  add_custom_command(OUTPUT ${example_resource_obj}
    COMMAND windres.exe ${windres_flags} -I. -i ${example_resource_rc} -o ${example_resource_obj}
    MAIN_DEPENDENCY ${example_resource_rc}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

# Function to add examples
function(add_vaca_example name)
  # Source files of the target
  file(GLOB example_sources ${name}/*.cpp)

  # Resource file to be used
  if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${name}/${name}.rc)
    # Particular resource file
    set(example_resource_rc ${CMAKE_CURRENT_SOURCE_DIR}/${name}/${name}.rc)
    set(example_resource_obj ${CMAKE_CURRENT_BINARY_DIR}/${name}.res.obj)

    # Custom rule to compile resources using windres.exe
    if(MINGW)
      add_custom_command(OUTPUT ${example_resource_obj}
	COMMAND windres.exe ${windres_flags} -I${name} -i ${name}/${name}.rc -o ${example_resource_obj}
	MAIN_DEPENDENCY ${example_resource_rc}
	WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
    endif()
  else()
    # General resource file
    set(example_resource_rc ${CMAKE_CURRENT_SOURCE_DIR}/Example.rc)
    set(example_resource_obj ${CMAKE_CURRENT_BINARY_DIR}/Example.res.obj)
  endif()

  if(MINGW)
    set(example_resource ${example_resource_obj})
  else()
    set(example_resource ${example_resource_rc})
  endif()

  # Add the Win32 target
  add_executable(${name} WIN32 ${example_sources} ${example_resource})

  # Setup compilation flags
  set_target_properties(${name} PROPERTIES
    COMPILE_FLAGS "${common_flags}")

  # Dependencies with libraries
  target_link_libraries(${name} Vaca)
  target_link_libraries(${name} ${win32_libs})

  # Custom command to embed our manifest file with MSVC compiler
  if(THEMES AND MSVC)
    # TODO The following custom command fails with VC solutions
    #      because we have to known object-output directory for each
    #      example (e.g. "examples/Buttons.dir/Release" instead of "")
    get_target_property(example_exe_output ${name} LOCATION)
    add_custom_command(TARGET ${name}
      POST_BUILD
      COMMAND mt.exe -nologo
		     -manifest ${CMAKE_CURRENT_SOURCE_DIR}/Example.manifest
			       ${CMAKE_CURRENT_BINARY_DIR}/${name}.exe.intermediate.manifest
		     -outputresource:${example_exe_output};#1
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
  endif()
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
