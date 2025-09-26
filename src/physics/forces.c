//
// Created by josue on 2024-07-29.
//

#include "forces.h"
#include "raymath.h"

void force_add_force(const Vector2 force, const float inverse_mass, Vector2 *out_forces) {
    *out_forces = Vector2Add(*out_forces, Vector2Scale(force, inverse_mass));
}

void force_apply_drag(const Vector2 velocity, const float coefficient, Vector2 *out_forces) {
    Vector2 drag = {0};
    const float velocity_magnitude_squared = Vector2LengthSqr(velocity);
    if(velocity_magnitude_squared > 0.0f) {
        const Vector2 direction = Vector2Negate(Vector2Normalize(velocity));
        const float magnitude = coefficient * velocity_magnitude_squared;
        drag = Vector2Scale(direction, magnitude);
    }
    *out_forces = Vector2Add(drag, *out_forces);
}
