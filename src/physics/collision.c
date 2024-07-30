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

bool circle_circle_collision_check(Body *a, Body *b, Contact *contact) {
    vec2 ab;
    glm_vec2_sub(b->position, a->position, ab);
    float radius_sum  = a->circle_shape.radius + b->circle_shape.radius;
    if (glm_vec2_norm2(ab) < glm_pow2(radius_sum)) {
        contact->a = a;
        contact->b = b;

        glm_vec2_normalize_to(ab, contact->normal); // collision normal
        contact->depth = (a->circle_shape.radius + b->circle_shape.radius) - glm_vec2_norm(ab);

        // start and end of the collision. Both points at the edge of each circle along the collision normal
        contact->start[0] = b->position[0] - contact->normal[0] * b->circle_shape.radius;
        contact->start[1] = b->position[1] - contact->normal[1] * b->circle_shape.radius;
        contact->end[0] = a->position[0] + contact->normal[0] * a->circle_shape.radius;
        contact->end[1] = a->position[1] + contact->normal[1] * a->circle_shape.radius;

        return true;
    }
    return false;
}

void resolve_collision(Contact contact) {
    contact.a->position[0] += contact.normal[0] * contact.depth * -0.5f + GLM_FLT_EPSILON;
    contact.a->position[1] += contact.normal[1] * contact.depth * -0.5f + GLM_FLT_EPSILON;
    contact.b->position[0] += contact.normal[0] * contact.depth * 0.5f + GLM_FLT_EPSILON;
    contact.b->position[1] += contact.normal[1] * contact.depth * 0.5f + GLM_FLT_EPSILON;

    glm_vec2_reflect(contact.a->linear_velocity, contact.normal, contact.a->linear_velocity);
    glm_vec2_reflect(contact.b->linear_velocity, (vec2){-contact.normal[0], -contact.normal[1]}, contact.b->linear_velocity);
}
