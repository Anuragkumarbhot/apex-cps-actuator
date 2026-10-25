cmake_minimum_required(VERSION 3.16)
project(apex_actuator_firmware C)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Werror")

include_directories(include)
add_library(foc_core STATIC src/foc_core.c)
target_link_libraries(foc_core m)

add_executable(test_foc tests/test_foc.c)
target_link_libraries(test_foc foc_core m)
enable_testing()
add_test(NAME FOC_Unit_Tests COMMAND test_foc)
