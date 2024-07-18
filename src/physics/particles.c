//
// Created by josue on 2024-07-17.
//

#include "particles.h"

Particle particle(const vec2 position, const float radius, const float mass, bool anchor) {
    Particle result = {
            .position = {position[0], position[1]},
            .velocity = {0},
            .radius = radius,
            .inverse_mass = 1.0f/ mass,
            .anchor = anchor,
    };
    return result;
}
