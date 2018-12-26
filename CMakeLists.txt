cmake_minimum_required(VERSION 3.11)
project(sort)

set(CMAKE_CXX_STANDARD 14)


add_executable(sort main.cpp)
add_executable(sum sum.cpp)
add_executable(generate generate.cpp)
add_executable(canon_sort canon_sort.cpp)
add_executable(radix radix.cpp PackedArray.c)
add_executable(sort_benchmark counting_sort_benchmark.cpp)
add_executable(parallel_write_test parallel_write_test.cpp)

ADD_SUBDIRECTORY (googletest)
enable_testing()
include_directories(googletest/googletest/include googletest/googletest)
#configure_file(CMakeLists.txt.in googletest/CMakeLists.txt)

add_executable(PackedArray_test PackedArray_test.cpp PackedArray.c)
target_link_libraries(PackedArray_test gtest gtest_main)
add_test( PackedArray_test PackedArray_test )

add_executable(PackedArray_my_test PackedArray_my_test.cpp)
target_link_libraries(PackedArray_my_test gtest gtest_main)
add_test( PackedArray_my_test PackedArray_my_test )
