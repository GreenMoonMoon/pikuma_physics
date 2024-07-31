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
    float depth_mass = contact.depth / (contact.a->inverse_mass + contact.b->inverse_mass);
    float depth_a = depth_mass * contact.b->inverse_mass;
    float depth_b = depth_mass * contact.a->inverse_mass;

    // Get relative velocity along normal
    vec2 relative_velocity;
    glm_vec2_sub(contact.a->linear_velocity, contact.b->linear_velocity, relative_velocity);
    glm_vec2_scale(relative_velocity, glm_vec2_dot(relative_velocity, contact.normal), relative_velocity);

    // apply coefficient of restitution
    vec2 new_relative_velocity_a;
    glm_vec2_scale(relative_velocity, -contact.a->restitution, new_relative_velocity_a);
    vec2 new_relative_velocity_b;
    glm_vec2_scale(relative_velocity, contact.b->restitution, new_relative_velocity_b);

    // resolve penetration
    glm_vec2_mulsubs(contact.normal, depth_a, contact.a->position);
    glm_vec2_muladds(contact.normal, depth_b, contact.b->position);

//    vec2 scaled_normal_a;
//    glm_vec2_scale(contact.normal, 1.0f, scaled_normal_a);
//    glm_vec2_reflect(contact.a->linear_velocity, scaled_normal_a, contact.a->linear_velocity);
//    vec2 scaled_normal_b;
//    glm_vec2_scale(contact.normal, -1.0f, scaled_normal_b);
//    glm_vec2_reflect(contact.b->linear_velocity, scaled_normal_b, contact.b->linear_velocity);

    vec2 rel_vel;
    glm_vec2_sub(contact.a->linear_velocity, contact.b->linear_velocity, rel_vel);

    float inverse_mass_sum = contact.a->inverse_mass + contact.b->inverse_mass;
    float restitution = 0.75f;
    float impulse_magnitude = -(1+restitution)*glm_vec2_dot(rel_vel, contact.normal) * inverse_mass_sum;

    vec2 impulse_a;
    glm_vec2_scale(contact.normal, impulse_magnitude, impulse_a);
    body_apply_impulse(contact.a, impulse_a);
    vec2 impulse_b;
    glm_vec2_scale(contact.normal, -impulse_magnitude, impulse_b);
    body_apply_impulse(contact.b, impulse_b);
}
