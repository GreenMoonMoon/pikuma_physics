//
// Created by moonmoon on 30/07/23.
//

#include "particles.h"

Particle::Particle(Vector2 position, float mass) : Position(position), Velocity(Vector2()), Acceleration(Vector2()), Mass(mass) {}

Particle::Particle(float x, float y, float mass) : Position(Vector2(x, y)), Velocity(Vector2()), Acceleration(Vector2()), Mass(mass){}

