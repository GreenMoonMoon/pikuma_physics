//
// Created by moonmoon on 30/07/23.
//

#ifndef PIKUMA_PHYSICS_PARTICLES_H
#define PIKUMA_PHYSICS_PARTICLES_H

#include "raymath.h"

struct Particle {
    Vector2 Position;
    Vector2 Velocity;
    Vector2 Acceleration;

    float Mass;
    Vector2 TotalForce;

    Particle(Vector2 position, float mass);
    Particle(float x, float y, float mass);
    ~Particle() = default;

    void Integrate(double deltaTime);
    void AddForce(const Vector2 &force);
    void ClearForces();
};

#endif //PIKUMA_PHYSICS_PARTICLES_H
