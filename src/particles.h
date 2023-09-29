//
// Created by moonmoon on 30/07/23.
//

#ifndef PIKUMA_PHYSICS_PARTICLES_H
#define PIKUMA_PHYSICS_PARTICLES_H

#include "glm/vec2.hpp"

struct Particle {
    glm::vec2 Position;
    glm::vec2 Velocity;
    glm::vec2 Acceleration;

    float Mass;
    float InverseMass;
    glm::vec2 TotalForce;

    Particle(glm::vec2 position, float mass);
    Particle(float x, float y, float mass);
    ~Particle() = default;

    void Integrate(float deltaTime);
    void AddForce(const glm::vec2 &force);
    void ClearForces();
};

#endif //PIKUMA_PHYSICS_PARTICLES_H
