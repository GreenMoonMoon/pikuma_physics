//
// Created by josue on 2024-07-29.
//

#include "forces.h"

void force_add_force(const vec2 force, float inverse_mass, vec2 out_forces) {
    glm_vec2_add(out_forces, (vec2){force[0] * inverse_mass, force[1] * inverse_mass}, out_forces);
}

void force_apply_drag(const vec2 velocity, float coefficient, vec2 out_forces) {
    vec2 drag = {0};
    vec2 direction;
    float velocity_magnitude_squared = glm_vec2_norm2(velocity);
    if(velocity_magnitude_squared > 0.0f) {
        glm_vec2_normalize_to(velocity, direction);
        glm_vec2_negate(direction);
        float magnitude = coefficient * velocity_magnitude_squared;
        glm_vec2_scale(direction, magnitude, drag);
    }
    glm_vec2_add(out_forces, drag, out_forces);
}
