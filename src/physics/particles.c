//
// Created by josue on 2024-07-17.
//

#include "particles.h"

Particle particle(const Vector2 position, const float radius, const float mass, const bool anchor) {
    const Particle result = {
            .position = position,
            .velocity = {0},
            .radius = radius,
            .inverse_mass = 1.0f/ mass,
            .anchor = anchor,
    };
    return result;
}
