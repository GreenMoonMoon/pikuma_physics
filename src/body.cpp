//
// Created by moonmoon on 30/07/23.
//

#include "body.h"
#include "glm/geometric.hpp"

using glm::vec2;

Body::Body(const Shape &shape, float x, float y, float mass, float angle) : Position(vec2(x, y)), Rotation(angle),
                                                                            Mass(mass) {
    this->shape = shape.Copy();
    this->I = shape.GetMomentOfInertia() * mass;
    if (mass != 0.0f) {
        InverseMass = 1.0f / mass;
    }
    if (this->I != 0) {
        InverseI = -1.0f / this->I;
    }
}

Body::~Body() {}

void Body::IntegrateLinear(float deltaTime) {
    // Find the acceleration from the forces applied
    Acceleration = TotalForce * InverseMass;

    // Integrate acceleration
    Velocity += Acceleration * deltaTime;

    // Integrate velocity
    Position += Velocity * deltaTime;

    ClearForces();
}

void Body::IntegrateAngular(float deltaTime) {
    // Find the angular acceleration form the total torque applied
    AngularAcceleration = TotalTorque * InverseI;

    // Integrate angular acceleration
    AngularVelocity += AngularAcceleration * deltaTime;

    // Integrate angular velocity
    Rotation += AngularVelocity * deltaTime;

    ClearTorques();
}

void Body::AddForce(const vec2 &force) {
    TotalForce += force;
}

void Body::AddTorque(float torque) {
    TotalTorque += torque;
}

void Body::ClearForces() {
    TotalForce = vec2(0.0f);
}

void Body::ClearTorques() {
    TotalTorque = 0.0f;
}

void Body::Update(float deltaTime) {
    IntegrateLinear(deltaTime);
    IntegrateAngular(deltaTime);
}
