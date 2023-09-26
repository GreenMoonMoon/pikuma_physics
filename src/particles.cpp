//
// Created by moonmoon on 30/07/23.
//

#include "particles.h"

Particle::Particle(vec2 position, float mass) : Position(position), Velocity(vec2()), Acceleration(vec2()), Mass(mass), InverseMass(0.0f), TotalForce(vec2()) {
    if(mass != 0.0f){
        InverseMass = 1.0f / mass;
    }
}

Particle::Particle(float x, float y, float mass) : Position(vec2(x, y)), Velocity(vec2()), Acceleration(vec2()), Mass(mass),  InverseMass(0.0f), TotalForce(vec2()) {
    if(mass != 0.0f) {
        InverseMass = 1.0f / mass;
    }
}

void Particle::Integrate(float deltaTime) {
    // Find the acceleration from the forces applied
    Acceleration = TotalForce * InverseMass;

    // Integrate acceleration
    Velocity = Velocity + Acceleration * deltaTime;

    // Integrate velocity
    Position = Position + Velocity * deltaTime;

    ClearForces();
}

void Particle::AddForce(const vec2 &force) {
    TotalForce += force;
}

void Particle::ClearForces() {
    TotalForce = vec2(0.0f);
}

