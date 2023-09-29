//
// Created by josue on 2023-09-26.
//

#ifndef PIKUMA_PHYSICS_FORCES_H
#define PIKUMA_PHYSICS_FORCES_H

#include "glm/vec2.hpp"
#include "particles.h"

namespace Force {
    glm::vec2 GenerateDragForce(const glm::vec2 &velocity, float dragCoefficient);
    glm::vec2 GenerateFrictionForce(const glm::vec2 &velocity, float frictionCoefficient);
    glm::vec2 GenerateGravitationalForce(const Particle &a, const Particle &b, float g);
}

#endif //PIKUMA_PHYSICS_FORCES_H
