cmake_minimum_required(VERSION 3.5)
project(pikuma_physics)

set(CMAKE_CXX_STANDARD 23)

include(FetchContent)
FetchContent_Declare(
        raylib
        GIT_REPOSITORY https://github.com/raysan5/raylib.git
        GIT_TAG 5.5
)
set(GLM_BUILD_TESTS OFF)
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG master
)
FetchContent_MakeAvailable(raylib glm)

add_executable(pikuma_physics src/main.cpp
        src/physics/forces.cpp
        src/physics/body.cpp
        src/physics/shape.cpp
        src/physics/collisions.cpp
        src/graphics/raylib_extension.cpp
        src/graphics/draw.cpp)
target_link_libraries(pikuma_physics PRIVATE raylib glm::glm)
target_include_directories(pikuma_physics PRIVATE extern)
