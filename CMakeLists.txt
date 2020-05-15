cmake_minimum_required(VERSION 3.7)

project(mau)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(third-party/fmt)
add_subdirectory(third-party/gl3w)
add_subdirectory(mau-framework)
add_subdirectory(worship)
