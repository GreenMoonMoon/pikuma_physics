//
// Created by josue on 2023-09-26.
//

#ifndef PIKUMA_PHYSICS_FORCES_H
#define PIKUMA_PHYSICS_FORCES_H

#include "glm/vec2.hpp"
#include "particles.h"

namespace Force {
    glm::vec2 GenerateDragForce(const Particle &p, float dragCoefficient);
    glm::vec2 GenerateFrictionForce(const Particle &p, float frictionCoefficient);
    glm::vec2 GenerateGravitationalForce(const Particle &a, const Particle &b, float g, float minDistance, float maxDistance);
    glm::vec2 GenerateSpringForce(const Particle &p, glm::vec2 anchor, float restLength, float springConstant);
    glm::vec2 GenerateSpringForce(const Particle &a, const Particle &b, float restLength, float springConstant);
}

#endif //PIKUMA_PHYSICS_FORCES_H
