//
// Created by josue on 2023-09-26.
//

#include "forces.h"

glm::vec2 Force::GenerateDragForce(const glm::vec2 &velocity, float dragCoefficient) {
    float magnitudeSquared = (velocity.x * velocity.x + velocity.y * velocity.y);
    if (magnitudeSquared > 0.0f) {
        return dragCoefficient * magnitudeSquared * (glm::normalize(velocity) * -1.0f);
    } else {
        return glm::vec2(0.0f);
    }
}

glm::vec2 Force::GenerateFrictionForce(const glm::vec2 &velocity, float frictionCoefficient) {
//    glm::vec2 force(0.0f);
    if ((velocity.x * velocity.x + velocity.y * velocity.y) != 0.0f) {
//        glm::vec2 direction = glm::normalize(velocity) * -1.0f;
//        float magnitude = frictionCoefficient;
//        force = direction * magnitude;
        return (glm::normalize(velocity) * -1.0f) * frictionCoefficient;
    }
//    return force;
    return glm::vec2(0.0f);
}
