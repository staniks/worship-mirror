cmake_minimum_required(VERSION 3.7)

project(fmt)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE fmt_SOURCES "src/*.cc")
add_library(fmt ${fmt_SOURCES})
target_include_directories(fmt PUBLIC "include")
