//
// Created by josue on 2023-09-26.
//

#ifndef PIKUMA_PHYSICS_FORCES_H
#define PIKUMA_PHYSICS_FORCES_H

#include "glm/vec2.hpp"
#include "glm/geometric.hpp"

namespace Force {
    inline static glm::vec2 GenerateDragForce(const glm::vec2 &velocity, float dragCoefficient){
        float magnitudeSquared = (velocity.x * velocity.x + velocity.y * velocity.y);
        if (magnitudeSquared > 0.0f){
            return dragCoefficient * magnitudeSquared * (glm::normalize(velocity) * -1.0f);
        } else {
            return vec2(0.0f);
        }
    }
}

#endif //PIKUMA_PHYSICS_FORCES_H
