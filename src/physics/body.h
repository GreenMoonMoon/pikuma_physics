//
// Created by moonmoon on 30/07/23.
//

#ifndef PIKUMA_PHYSICS_BODY_H
#define PIKUMA_PHYSICS_BODY_H

#include <memory>
#include "glm/vec2.hpp"
#include "shape.h"

struct Body {
    // Linear Motion
    glm::vec2 Position;
    glm::vec2 Velocity = glm::vec2();
    glm::vec2 Acceleration = glm::vec2();

    // Angular Motion
    float Rotation;
    float AngularVelocity = 0.0f;
    float AngularAcceleration = 0.0f;

    // Mass and Force
    float Mass;
    float InverseMass = 0.0f;
    glm::vec2 TotalForce = glm::vec2();

    // Moment of Inertia and Torque
    float I = 0.0f;
    float InverseI = 0.0f;
    float TotalTorque = 0.0f;

    std::shared_ptr<Shape> shape;

    bool IsColliding = false;

    Body(const Shape &shape, float x, float y, float mass, float angle);
    ~Body() = default;

    void IntegrateLinear(float deltaTime);
    void IntegrateAngular(float deltaTime);

    // Forces
    void AddForce(const glm::vec2 &force);
    void AddTorque(float torque);
    void ClearForces();
    void ClearTorques();

    void Update(float deltaTime);

    bool IsStatic() const;
};

#endif //PIKUMA_PHYSICS_BODY_H
