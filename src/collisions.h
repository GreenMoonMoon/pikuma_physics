//
// Created by josue on 2023-10-10.
//

#ifndef PIKUMA_PHYSICS_COLLISIONS_H
#define PIKUMA_PHYSICS_COLLISIONS_H

#include "body.h"
#include "glm/vec2.hpp"

struct Contact {
    const Body *a;
    const Body *b;

    glm::vec2 start;
    glm::vec2 end;

    glm::vec2 normal;
    float depth;

    Contact() = default;
    ~Contact() = default;
};

namespace CollisionDetection {
    bool IsColliding(const Body &a, const Body &b, Contact &contact);

    bool IsCollidingCircleCircle(const Body &a, const Body &b, Contact &contact);
}

#endif //PIKUMA_PHYSICS_COLLISIONS_H
