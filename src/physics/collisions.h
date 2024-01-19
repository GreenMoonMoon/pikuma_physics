//
// Created by josue on 2023-10-10.
//

#ifndef PIKUMA_PHYSICS_COLLISIONS_H
#define PIKUMA_PHYSICS_COLLISIONS_H

#include "body.h"
#include "glm/vec2.hpp"

struct Contact {
    Body *a;
    Body *b;

    glm::vec2 start;
    glm::vec2 end;

    glm::vec2 normal;
    float depth;

    Contact() = default;
    ~Contact() = default;
};

namespace Collision {
    bool IsColliding(Body &a, Body &b, Contact &contact);

    bool IsCollidingCircleCircle(Body &body_a, Body &body_b, Contact &contact);

    bool IsCollidingPolygonPolygon(const Body &body_a, const Body &body_b, Contact &contact);

    void ResolvePenetration(Contact &contact);

    void ResolveCollision(Contact &contact);

}

#endif //PIKUMA_PHYSICS_COLLISIONS_H
