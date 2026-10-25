cmake_minimum_required(VERSION 3.10)
project(ApexActuatorFirmware C)
enable_testing()

include_directories(include)

add_executable(test_biquad tests/test_biquad.c)
target_link_libraries(test_biquad m)
add_test(NAME BiquadFilterTest COMMAND test_biquad)

add_executable(test_cascade_thermal tests/test_cascade_thermal.c src/pid_cascade.c)
target_link_libraries(test_cascade_thermal m)
add_test(NAME CascadeThermalTest COMMAND test_cascade_thermal)
