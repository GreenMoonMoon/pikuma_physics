cmake_minimum_required(VERSION 3.26)
project(pikuma_physics)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FETCHCONTENT_DECLARE(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 4.5.0
)
FETCHCONTENT_MAKEAVAILABLE(raylib)

add_executable(pikuma_physics main.cpp)
target_link_libraries(pikuma_physics PRIVATE raylib)
