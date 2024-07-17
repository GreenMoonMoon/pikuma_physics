cmake_minimum_required(VERSION 3.26)
project(pikuma_physics)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.0
        GIT_PROGRESS TRUE
)
FetchContent_MakeAvailable(raylib)
FetchContent_Declare(
        cglm
        GIT_REPOSITORY https://github.com/recp/cglm.git
        GIT_TAG v0.9.4
)
if(NOT cglm_POPULATED) # Make CGLM available
    FetchContent_Populate(cglm)
    include_directories(${cglm_SOURCE_DIR}/include)
endif ()

include_directories(src)

add_executable(pikuma_physics src/main.c
        src/physics/forces.cpp
        src/physics/body.cpp
        src/physics/shape.c
        src/physics/collisions.cpp
        src/graphics/raylib_extension.cpp
        src/graphics/draw.cpp)
target_link_libraries(pikuma_physics PRIVATE raylib)
target_include_directories(pikuma_physics PRIVATE src)
