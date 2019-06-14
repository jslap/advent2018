cmake_minimum_required(VERSION 2.8.12)
project(my_project)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(abseil-cpp)
add_subdirectory(fmt)
# add_subdirectory(range-v3)

include_directories("range-v3/include")
include_directories("fmt/include")
include_directories("abseil-cpp")

set(CMAKE_CXX_FLAGS "-O0 ${CMAKE_CXX_FLAGS} -Werror -Wall -Wpedantic -Wno-unused-private-field")

add_executable(my_exe dec16.cpp)
target_link_libraries(my_exe absl::base absl::strings absl::numeric fmt)
