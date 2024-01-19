//
// Created by josue on 2023-10-10.
//

#include "collisions.h"
#include "glm/vec2.hpp"
#include "glm/geometric.hpp"
#include "glm/gtx/perpendicular.hpp"

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

float find_min_sep(const PolygonShape &a, const PolygonShape &b) {
    float separation = std::numeric_limits<float>::lowest();

    // Loop all vertices from a
    for (int i = 0; i < a.WorldVertices.size(); i++) {
        glm::vec2 va = a.WorldVertices[i];
        glm::vec2 edge = a.EdgeAt(i);
        glm::vec2 normal(edge.y, -edge.x);

        float min_sep = std::numeric_limits<float>::max();
        for(auto vb : b.WorldVertices) {
            min_sep = glm::min(min_sep, glm::dot((vb - va), normal));
        }

        separation = glm::max(separation, min_sep);
    }

    return separation;
}

bool Collision::IsCollidingPolygonPolygon(const Body& body_a, const Body& body_b, Contact& contact) {
    // bool result = dynamic_cast<PolygonShape*>(a.shape.get())->FindMinimumSeparation(dynamic_cast<PolygonShape*>(b.shape.get())) <= 0
    // && dynamic_cast<PolygonShape*>(b.shape.get())->FindMinimumSeparation(dynamic_cast<PolygonShape*>(a.shape.get())) <= 0;

    const auto* shapeA = dynamic_cast<PolygonShape *>(body_a.shape.get());
    const auto* shapeB = dynamic_cast<PolygonShape *>(body_b.shape.get());
    if (find_min_sep(*shapeA, *shapeB) >= 0) return false;
    if (find_min_sep(*shapeB, *shapeA) >= 0) return false;

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
