//
// Created by moonmoon on 30/07/23.
//

#include "body.h"

using glm::vec2;

Body::Body(const Shape &shape, float x, float y, float mass) : Position(vec2(x, y)), Velocity(vec2()), Acceleration(vec2()), Mass(mass), TotalForce(vec2()) {
    this->shape = shape.Copy();
    if(mass != 0.0f) {
        InverseMass = 1.0f / mass;
    }
}

Body::~Body() {
    delete shape;
}

void Body::Integrate(float deltaTime) {
    // Find the acceleration from the forces applied
    Acceleration = TotalForce * InverseMass;

    // Integrate acceleration
    Velocity = Velocity + Acceleration * deltaTime;

    // Integrate velocity
    Position = Position + Velocity * deltaTime;

    ClearForces();
}

void Body::AddForce(const vec2 &force) {
    TotalForce += force;
}

void Body::ClearForces() {
    TotalForce = vec2(0.0f);
}
