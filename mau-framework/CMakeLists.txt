cmake_minimum_required(VERSION 3.7)

project(mau-framework)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(OpenGL_GL_PREFERENCE GLVND)

find_package(SDL2 REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(OpenGL 3.3 REQUIRED)

set(MAU_INCLUDES
    ${SDL2_INCLUDE_DIRS}
    ${SDL2_mixer_INCLUDE_DIRS}
    include
	${CMAKE_SOURCE_DIR}/third-party/glm
	${CMAKE_SOURCE_DIR}/third-party/Catch
    ${CMAKE_SOURCE_DIR}/third-party/frozen/include
)

set(MAU_LIBRARIES
    SDL2
    SDL2main
    SDL2_mixer
    ${OPENGL_LIBRARIES}
    ${CMAKE_DL_LIBS}
    stdc++fs
    fmt
    gl3w
)

file(GLOB_RECURSE mau_SOURCES "src/*.cpp")
add_library(mau ${mau_SOURCES})
target_include_directories(mau PUBLIC ${MAU_INCLUDES})
target_link_libraries(mau PUBLIC ${MAU_LIBRARIES})
