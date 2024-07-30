//
// Created by josue on 2024-07-29.
//

#include "collision.h"
#include "rigidbodies.h"

void circle_check_resolve_boundary(Body *body, const vec2 min, const vec2 max) {
    if (body->position[0]-body->circle_shape.radius < min[0]) {
        body->position[0] = body->circle_shape.radius + min[0];
        body->linear_velocity[0] = -body->linear_velocity[0] * 0.75f;
    } else if (body->position[0]+body->circle_shape.radius > max[0]){
        body->position[0] = max[0] - body->circle_shape.radius;
        body->linear_velocity[0] = -body->linear_velocity[0] * 0.75f;
    }
    if (body->position[1]-body->circle_shape.radius < min[1]) {
        body->position[1] = body->circle_shape.radius + min[1];
        body->linear_velocity[1] = -body->linear_velocity[1] * 0.75f;
    } else if (body->position[1]+body->circle_shape.radius > max[1]){
        body->position[1] = max[1] - body->circle_shape.radius;
        body->linear_velocity[1] = -body->linear_velocity[1] * 0.75f;
    }
}

bool circle_circle_collision_check(Body *a, Body *b) {
    return (glm_vec2_distance2(a->position, b->position) < glm_pow2(a->circle_shape.radius + b->circle_shape.radius));
}

Collision circle_circle_get_collision_info(struct Body *a, struct Body *b) {
    Collision collision = {0};
    collision.a = a;
    collision.b = b;
    glm_vec2_sub(a->position, b->position, collision.normal); // direction
    glm_vec2_normalize(collision.normal);
    collision.depth = (a->circle_shape.radius + a->circle_shape.radius) - glm_vec2_distance(a->position, b->position);
    collision.start[0] = b->position[0] + collision.normal[0] * b->circle_shape.radius;
    collision.start[1] = b->position[1] + collision.normal[1] * b->circle_shape.radius;
    collision.end[0] = a->position[0] - collision.normal[0] * a->circle_shape.radius;
    collision.end[1] = a->position[1] - collision.normal[1] * a->circle_shape.radius;

    return collision;
}
