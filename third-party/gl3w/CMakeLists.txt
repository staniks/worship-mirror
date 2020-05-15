cmake_minimum_required(VERSION 3.7)

project(gl3w)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE gl3w_SOURCES "src/*.c")
add_library(gl3w ${gl3w_SOURCES})
target_include_directories(gl3w PUBLIC "include")
target_link_libraries(gl3w PUBLIC ${CMAKE_DL_LIBS})
