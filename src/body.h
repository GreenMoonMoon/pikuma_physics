//
// Created by moonmoon on 30/07/23.
//

#ifndef PIKUMA_PHYSICS_BODY_H
#define PIKUMA_PHYSICS_BODY_H

#include "glm/vec2.hpp"
#include "shape.h"

struct Body {
    glm::vec2 Position;
    glm::vec2 Velocity;
    glm::vec2 Acceleration;

    Shape* shape = nullptr;

    float Mass;
    float InverseMass = 0.0f;
    glm::vec2 TotalForce;

    Body(const Shape &shape, float x, float y, float mass);
    ~Body();

    void Integrate(float deltaTime);
    void AddForce(const glm::vec2 &force);
    void ClearForces();
};

#endif //PIKUMA_PHYSICS_BODY_H
