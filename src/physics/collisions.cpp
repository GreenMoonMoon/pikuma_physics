//
// Created by josue on 2023-10-10.
//

#include "collisions.h"
#include "glm/vec2.hpp"
#include "glm/geometric.hpp"

using glm::vec2;

bool CollisionDetection::IsColliding(Body &a, Body &b, Contact &contact) {
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

bool CollisionDetection::IsCollidingCircleCircle(Body &a, Body &b, Contact &contact) {
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

void Contact::ResolvePenetration() const {
    if(a->IsStatic() && b->IsStatic()) return;

    float displaceA = (depth / (a->InverseMass + b->InverseMass)) * a->InverseMass;
    float displaceB = (depth / (a->InverseMass + b->InverseMass)) * b->InverseMass;

    a->Position -= normal * displaceA;
    b->Position += normal * displaceB;
}
