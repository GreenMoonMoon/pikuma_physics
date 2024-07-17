//
// Created by josue on 2024-07-17.
//

#include "particles.h"

Particle particle(const vec2 position, const float radius, const float mass) {
    Particle result = {
            .position = {position[0], position[1]},
            .velocity = {0},
            .radius = radius,
            .inverse_mass = 1.0f/ mass,
    };
    return result;
}
