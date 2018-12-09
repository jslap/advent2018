cmake_minimum_required(VERSION 2.8.12)
project(my_project)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "-O0 ${CMAKE_CXX_FLAGS}")

add_subdirectory(abseil-cpp)
add_subdirectory(fmt)

add_executable(my_exe dec7.cpp)
target_link_libraries(my_exe absl::base absl::strings fmt)
