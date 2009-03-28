# Vaca - Visual Application Components Abstraction
# Copyright (c) 2005-2009 David Capello
# All rights reserved.

function(add_vaca_test name)
  add_executable(${name} ${name}.cpp)

  set_target_properties(${name} PROPERTIES
    COMPILE_FLAGS "${common_flags}")

  target_link_libraries(${name} Vaca)
  target_link_libraries(${name} ${win32_libs})
endfunction(add_vaca_test)

add_vaca_test(test_backward)
add_vaca_test(test_bind)
add_vaca_test(test_handle)
add_vaca_test(test_menu)
add_vaca_test(test_point)
add_vaca_test(test_rect)
add_vaca_test(test_region)
add_vaca_test(test_sharedptr)
add_vaca_test(test_signal)
add_vaca_test(test_size)
add_vaca_test(test_string)
add_vaca_test(test_tab)
add_vaca_test(test_thread)
add_vaca_test(test_widget)
