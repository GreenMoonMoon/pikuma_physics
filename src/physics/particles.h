//
// Created by josue on 2024-07-17.
//

#ifndef PIKUMA_PHYSICS_PARTICLES_H
#define PIKUMA_PHYSICS_PARTICLES_H

#include "raylib.h"

typedef struct Particle {
    Vector2 position;
    Vector2 velocity;
    float radius;
    float inverse_mass;
    bool anchor;
} Particle;

Particle particle(Vector2 position, float radius, float mass, bool anchor);

#endif //PIKUMA_PHYSICS_PARTICLES_H
