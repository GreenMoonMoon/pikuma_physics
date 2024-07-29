//
// Created by josue on 2024-07-29.
//

#include "forces.h"

void apply_drag_force(const vec2 velocity, float coefficient, vec2 forces) {
    vec2 drag = {0};
    vec2 direction;
    float velocity_magnitude_squared = glm_vec2_norm2(velocity);
    if(velocity_magnitude_squared > 0.0f) {
        glm_vec2_normalize_to(velocity, direction);
        glm_vec2_negate(direction);
        float magnitude = coefficient * velocity_magnitude_squared;
        glm_vec2_scale(direction, magnitude, drag);
    }
    glm_vec2_add(forces, drag, forces);
}