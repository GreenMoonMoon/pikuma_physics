//
// Created by josue on 2023-10-10.
//

#include "collisions.h"
#include "glm/vec2.hpp"
#include "glm/geometric.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/perpendicular.hpp"

using glm::vec2;

bool Collision::IsColliding(Body &a, Body &b, Contact &contact) {
    const Shape *shapeA = a.shape.get();
    const Shape *shapeB = b.shape.get();

    switch (shapeA->GetType()) {
        case CIRCLE:
            switch (shapeB->GetType()) {
                case CIRCLE:
                    return IsCollidingCircleCircle(a, b, contact);
                case POLYGON:
                    break;
                case BOX:
                    break;
            }
            break;
        case BOX:
        case POLYGON:
            switch (shapeB->GetType()) {
                case CIRCLE:
                    break;
                case BOX:
                case POLYGON:
                    return IsCollidingPolygonPolygon(a, b, contact);
                    break;
            }
            break;
    }

    return false;
}

bool Collision::IsCollidingCircleCircle(Body &body_a, Body &body_b, Contact &contact) {
    const auto* shapeA = dynamic_cast<CircleShape *>(body_a.shape.get());
    const auto* shapeB = dynamic_cast<CircleShape *>(body_b.shape.get());

    const vec2 direction = body_b.Position - body_a.Position;
    const float depth = glm::length(direction);

    if (const float radiusSum = shapeA->Radius + shapeB->Radius; depth > radiusSum) {
        return false;
    }

    contact.a = &body_a;
    contact.b = &body_b;

    contact.normal = glm::normalize(direction);
    contact.start = body_b.Position - contact.normal * shapeB->Radius;
    contact.end = body_a.Position + contact.normal * shapeA->Radius;

    contact.depth = glm::length(contact.end - contact.start);

    return true;
}

bool Collision::IsCollidingPolygonPolygon(Body& body_a, Body& body_b, Contact& contact) {
    const auto* shapeA = dynamic_cast<PolygonShape *>(body_a.shape.get());
    const auto* shapeB = dynamic_cast<PolygonShape *>(body_b.shape.get());

    vec2 axis_a;
    vec2 axis_b;
    vec2 point_a;
    vec2 point_b;
    const float separation_ab = shapeA->FindMinimumSeparation(*shapeB, axis_a, point_a);
    if (separation_ab >= 0) { return false; }

    const float separation_ba = shapeB->FindMinimumSeparation(*shapeA, axis_b, point_b);
    if (separation_ba >= 0) { return false; }

    // Populate contact information
    contact.a = &body_a;
    contact.b = &body_b;
    if(separation_ab > separation_ba) {
        contact.start = point_a;
        contact.normal = glm::normalize(glm::vec2(axis_a.y, -axis_a.x));
        contact.depth = -separation_ab;
        contact.end = contact.start + contact.normal * contact.depth;
    } else {
        contact.start = point_b;
        contact.normal = -glm::normalize(glm::vec2(axis_b.y, -axis_b.x));
        contact.depth = -separation_ba;
        contact.end = contact.start + contact.normal * contact.depth;
    }

    return true;
}

void Collision::ResolvePenetration(Contact &contact) {
    Body *a = contact.a;
    Body *b = contact.b;

    if(a->IsStatic() && b->IsStatic()) return;

    float displaceA = (contact.depth / (a->InverseMass + b->InverseMass)) * a->InverseMass;
    float displaceB = (contact.depth / (a->InverseMass + b->InverseMass)) * b->InverseMass;

    a->Position -= contact.normal * displaceA;
    b->Position += contact.normal * displaceB;
}

void Collision::ResolveCollision(Contact &contact) {
    Collision::ResolvePenetration(contact);

    Body *a = contact.a;
    Body *b = contact.b;

    float restitution = std::min(a->restitution, b->restitution);
    vec2 relativeVelocity = a->Velocity - b->Velocity;

    float impulse = -(1.0f + restitution) * glm::dot(relativeVelocity, contact.normal) / (a->InverseMass + b->InverseMass);
    a->ApplyImpulse(contact.normal * impulse);
    b->ApplyImpulse(contact.normal * -impulse);
}
