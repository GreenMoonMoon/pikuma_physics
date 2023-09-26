//
// Created by moonmoon on 30/07/23.
//

#ifndef PIKUMA_PHYSICS_PARTICLES_H
#define PIKUMA_PHYSICS_PARTICLES_H

#include "glm/vec2.hpp"
using glm::vec2;

struct Particle {
    vec2 Position;
    vec2 Velocity;
    vec2 Acceleration;

    float Mass;
    float InverseMass;
    vec2 TotalForce;

    Particle(vec2 position, float mass);
    Particle(float x, float y, float mass);
    ~Particle() = default;

    void Integrate(float deltaTime);
    void AddForce(const vec2 &force);
    void ClearForces();
};

#endif //PIKUMA_PHYSICS_PARTICLES_H
