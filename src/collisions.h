//
// Created by josue on 2023-10-10.
//

#ifndef PIKUMA_PHYSICS_COLLISIONS_H
#define PIKUMA_PHYSICS_COLLISIONS_H

#include "body.h"
#include "glm/vec2.hpp"

namespace CollisionDetection {
    struct Contact {
        Body *a;
        Body *b;

        glm::vec2 start;
        glm::vec2 end;

        glm::vec2 normal;
        float depth;
    };


    bool IsColliding(const Body &a, const Body &b);

    bool IsCollidingCircleCircle(const Body &a, const Body &b);
}

#endif //PIKUMA_PHYSICS_COLLISIONS_H
