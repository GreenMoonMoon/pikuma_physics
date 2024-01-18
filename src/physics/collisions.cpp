//
// Created by josue on 2023-10-10.
//

#include "collisions.h"
#include "glm/vec2.hpp"
#include "glm/geometric.hpp"

using glm::vec2;

bool Collision::IsColliding(Body &a, Body &b, Contact &contact) {
    Shape *shapeA = a.shape.get();
    Shape *shapeB = b.shape.get();

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
        case POLYGON:
            break;
        case BOX:
            break;
    }

    return false;
}

bool Collision::IsCollidingCircleCircle(Body &a, Body &b, Contact &contact) {
    auto *shapeA = dynamic_cast<CircleShape*>(a.shape.get());
    auto *shapeB = dynamic_cast<CircleShape*>(b.shape.get());
    const vec2 direction = b.Position - a.Position;
    const float depth = glm::length(direction);

    const float radiusSum = dynamic_cast<CircleShape *>(shapeA)->Radius + dynamic_cast<CircleShape *>(shapeB)->Radius;

    if (depth > radiusSum) {
        return false;
    }

    contact.a = &a;
    contact.b = &b;

    contact.normal = glm::normalize(direction);
    contact.start = b.Position - contact.normal * shapeB->Radius;
    contact.end = a.Position + contact.normal * shapeA->Radius;

    contact.depth = glm::length(contact.end - contact.start);

    return true;
}

bool Collision::IsCollidingPolygonPolygon(Body& a, Body& b, Contact& contact) {
    bool result = dynamic_cast<PolygonShape*>(a.shape.get())->FindMinimumSeparation(dynamic_cast<PolygonShape*>(b.shape.get())) <= 0
    && dynamic_cast<PolygonShape*>(b.shape.get())->FindMinimumSeparation(dynamic_cast<PolygonShape*>(a.shape.get())) <= 0;

    return result;
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
