//
// Created by moonmoon on 30/07/23.
//

#include "particles.h"

Particle::Particle(Vector2 position, float mass) : Position(position), Velocity(Vector2()), Acceleration(Vector2()), Mass(mass), InverseMass(1.0f), TotalForce(Vector2()) {
    if(mass != 0.0f){
        InverseMass = 1.0f / mass;
    }
}

Particle::Particle(float x, float y, float mass) : Position(Vector2(x, y)), Velocity(Vector2()), Acceleration(Vector2()), Mass(mass),  InverseMass(1.0f), TotalForce(Vector2()) {
    if(mass != 0.0f) {
        InverseMass = 1.0f / mass;
    }
}

void Particle::Integrate(double deltaTime) {
    // Find the acceleration from the forces applied
    Acceleration = Vector2Scale(TotalForce, InverseMass);

    // Integrate acceleration
    Velocity = Vector2Add(Velocity, Vector2Scale(Acceleration, deltaTime));

    // Integrate velocity
    Position = Vector2Add(Position, Vector2Scale(Velocity, deltaTime));

    ClearForces();
}

void Particle::AddForce(const Vector2 &force) {
    TotalForce = Vector2Add(TotalForce, force);
}

void Particle::ClearForces() {
    TotalForce = Vector2Zero();
}

