include_directories(../src)

add_executable(test_collisions test_sat.c)
target_sources(test_collisions PRIVATE
        ../src/physics/collision.c
        ../src/physics/rigidbodies.c
)
target_link_libraries(test_collisions PRIVATE raylib)