cmake_minimum_required(VERSION 3.26)
project(pikuma_physics)

set(CMAKE_C_STANDARD 23)

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
        src/physics/particles.c
        src/physics/rigidbodies.c

        src/scenes/particles_scene.c
        src/scenes/rigidbodies_scene.c

        src/graphics/raylib_utils.c
)
target_link_libraries(pikuma_physics PRIVATE raylib)
