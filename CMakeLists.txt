cmake_minimum_required(VERSION 3.26)
project(pikuma_physics)

set(CMAKE_C_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
        GIT_PROGRESS TRUE
)
FetchContent_Declare(
        cglm
        GIT_REPOSITORY https://github.com/recp/cglm.git
        GIT_TAG v0.9.6
)
FetchContent_MakeAvailable(raylib cglm)

add_executable(pikuma_physics src/main.c
        src/physics/particles.c
        src/physics/rigidbodies.c
        src/physics/forces.c
        src/physics/collision.c

        src/scenes/particles_scene.c
        src/scenes/rigidbodies_scene.c
)
target_link_libraries(pikuma_physics PRIVATE raylib cglm)
