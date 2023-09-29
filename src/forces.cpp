//
// Created by josue on 2023-09-26.
//

#include "forces.h"
#include "glm/geometric.hpp"

using glm::vec2;

static float length2(vec2 v) {
    return (v.x * v.x + v.y * v.y);
}

vec2 Force::GenerateDragForce(const Particle &p, float dragCoefficient) {
    float magnitudeSquared = length2(p.Velocity);
    if (magnitudeSquared > 0.0f) {
        return dragCoefficient * magnitudeSquared * (glm::normalize(p.Velocity) * -1.0f);
    } else {
        return vec2(0.0f);
    }
}

vec2 Force::GenerateFrictionForce(const Particle &p, float frictionCoefficient) {
    if (length2(p.Velocity) != 0.0f) {
        return (glm::normalize(p.Velocity) * -1.0f) * frictionCoefficient;
    }
    return vec2(0.0f);
}

vec2 Force::GenerateGravitationalForce(const Particle &a, const Particle &b, float g, float minDistance, float maxDistance) {
    vec2 direction = b.Position - a.Position;
    float distanceSquared = length2(direction);

    vec2 attractionDirection = glm::normalize(direction);
    float attractionMagnitude = g * (a.Mass * b.Mass) / glm::clamp(distanceSquared, minDistance, maxDistance);

    return attractionDirection * attractionMagnitude;
}

glm::vec2 Force::GenerateSpringForce(const Particle &p, vec2 anchor, float restLength, float springConstant) {
    vec2 distance = p.Position - anchor;
    float displacement = glm::length(distance) - restLength;

    vec2 springDirection = glm::normalize(distance);
    float springMagnitude = -springConstant * displacement;

    return springDirection * springMagnitude;
}

glm::vec2 Force::GenerateSpringForce(const Particle &a, const Particle &b, float restLength, float springConstant) {
    vec2 distance = a.Position - b.Position;
    float displacement = glm::length(distance) - restLength;

    vec2 springDirection = glm::normalize(distance);
    float springMagnitude = -springConstant * displacement;

    return springDirection * springMagnitude;
}
