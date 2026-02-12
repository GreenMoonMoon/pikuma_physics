//
// Created by green on 2026-02-11.
//

#include "tst.h"
#include "raylib.h"
#include "raymath.h"
#include "physics/collision.h"
#include "physics/rigidbodies.h"

Vector2 vertices[9] = {
    {-10.0f, -10.f}, {-5.0f, 0.0f}, {0.0f, -10.0f},
    {-5.0f, -10.0f}, {0.0f, 0.0f}, {5.0f, -10.0f},
    {6.0f, 0.0f}, {11.0f, 10.0f}, {16.0f, 0.0f}
};
PolygonShape shape_a = {.vertices = vertices, .vertex_count = 3};
PolygonShape shape_b = {.vertices = vertices + 3, .vertex_count = 3};
PolygonShape shape_c = {.vertices = vertices + 6, .vertex_count = 3};

tstsuite("Colliding polygons tests") {
    tstcase("simple polygon colliding") {
        const float separation = get_minimum_separation(&shape_a, &shape_b); // minimum separation
        tstprintf("separation is %f\n", separation);
        tstcheck(!(separation > 0.0f), "separation should be lesser than 0: is %f", separation)
    }
    tstcase("simple polygon not colliding") {
        const float separation = get_minimum_separation(&shape_a, &shape_c);
        tstprintf("separation is %f\n", separation);
        tstcheck(separation > 0.0f, "separation should be greater than 0: is %f", separation);
    }
}