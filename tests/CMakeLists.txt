# Vaca - Visual Application Components Abstraction
# Copyright (c) 2005-2010 David Capello
# All rights reserved.

# Fix to compile gtest with VC11 (2012)
if (MSVC_VERSION EQUAL 1700)
  add_definitions(-D_VARIADIC_MAX=10)
endif()

include_directories(.)

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/3655149a60ad7bfeb0902f7308b95dafa97a68ad.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

function(add_vaca_test name)
  add_executable(${name} ${name}.cpp)

  set_target_properties(${name} PROPERTIES
    COMPILE_FLAGS "${common_flags}")

  target_link_libraries(${name} gtest_main vaca)

  add_test(NAME ${name} COMMAND ${name})
endfunction(add_vaca_test)

add_vaca_test(test_handle)
add_vaca_test(test_image)
add_vaca_test(test_menu)
add_vaca_test(test_pen)
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
