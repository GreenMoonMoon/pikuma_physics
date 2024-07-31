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
        glm_vec2_mulsubs(contact->normal, b->circle_shape.radius, contact->start);
        glm_vec2_add(contact->start, b->position, contact->start);
        glm_vec2_muladds(contact->normal, a->circle_shape.radius, contact->end);
        glm_vec2_add(contact->end, a->position, contact->end);

        return true;
    }
    return false;
}

void resolve_collision(Contact contact) {
    float inverse_mass_sum = contact.a->inverse_mass + contact.b->inverse_mass;
    float depth_mass = contact.depth / inverse_mass_sum;
    float depth_a = depth_mass * contact.b->inverse_mass;
    float depth_b = depth_mass * contact.a->inverse_mass;

    // resolve penetration
    glm_vec2_mulsubs(contact.normal, depth_a, contact.a->position);
    glm_vec2_muladds(contact.normal, depth_b, contact.b->position);

    vec2 relative_velocity;
    glm_vec2_sub(contact.a->linear_velocity, contact.b->linear_velocity, relative_velocity);

    float e = fminf(contact.a->restitution, contact.b->restitution);
    float impulse_magnitude = -(1 + e) * glm_vec2_dot(relative_velocity, contact.normal) / inverse_mass_sum;

    vec2 impulse_a;
    glm_vec2_scale(contact.normal, impulse_magnitude, impulse_a);
    body_apply_impulse(contact.a, impulse_a);
    vec2 impulse_b;
    glm_vec2_scale(contact.normal, -impulse_magnitude, impulse_b);
    body_apply_impulse(contact.b, impulse_b);
}
