//
// Created by josue on 2023-09-26.
//

#include "forces.h"
#include "glm/gtx/norm.hpp"

using glm::vec2;

vec2 Force::GenerateDragForce(const vec2 &velocity, float dragCoefficient) {
    float magnitudeSquared = glm::length2(velocity);
    if (magnitudeSquared > 0.0f) {
        return dragCoefficient * magnitudeSquared * (glm::normalize(velocity) * -1.0f);
    } else {
        return vec2(0.0f);
    }
}

vec2 Force::GenerateFrictionForce(const vec2 &velocity, float frictionCoefficient) {
    if (glm::length2(velocity) != 0.0f) {
        return (glm::normalize(velocity) * -1.0f) * frictionCoefficient;
    }
    return vec2(0.0f);
}

vec2 Force::GenerateGravitationalForce(const Particle &a, const Particle &b, float g, float minDistance, float maxDistance) {
    vec2 direction = b.Position - a.Position;
    float distanceSquared = glm::length2(direction);

    vec2 attractionDirection = glm::normalize(direction);
    float attractionMagnitude = g * (a.Mass * b.Mass) / glm::clamp(distanceSquared, minDistance, maxDistance);

    return attractionDirection * attractionMagnitude;
}
