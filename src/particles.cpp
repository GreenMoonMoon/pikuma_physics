//
// Created by moonmoon on 30/07/23.
//

#include "particles.h"

Particle::Particle(Vector2 position, float mass) : Position(position), Velocity(Vector2()), Acceleration(Vector2()), Mass(mass) {}

Particle::Particle(float x, float y, float mass) : Position(Vector2(x, y)), Velocity(Vector2()), Acceleration(Vector2()), Mass(mass){}

void Particle::Integrate(double deltaTime) {
    // Integrate acceleration
    Velocity = Vector2Add(Velocity, Vector2Scale(Acceleration, deltaTime));

    // Integrate velocity
    Position = Vector2Add(Position, Vector2Scale(Velocity, deltaTime));
}

