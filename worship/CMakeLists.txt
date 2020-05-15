cmake_minimum_required(VERSION 3.7)

project(worship)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB_RECURSE worship_SOURCES "src/*.cpp")
add_executable(worship ${worship_SOURCES})
target_include_directories(worship PUBLIC include)
target_link_libraries(worship PUBLIC mau ${MAU_LIBRARIES})
