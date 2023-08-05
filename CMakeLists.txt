cmake_minimum_required(VERSION 3.26)
project(pikuma_physics)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.5.0
)
FetchContent_MakeAvailable(raylib)

add_executable(pikuma_physics src/main.cpp
        src/vectors.cpp
        src/particles.cpp
        src/particles.h)
target_link_libraries(pikuma_physics PRIVATE raylib)
