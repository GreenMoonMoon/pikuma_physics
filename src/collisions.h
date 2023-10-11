//
// Created by josue on 2023-10-10.
//

#ifndef PIKUMA_PHYSICS_COLLISIONS_H
#define PIKUMA_PHYSICS_COLLISIONS_H

#include "body.h"


namespace CollisionDetection {
    bool IsColliding(const Body &a, const Body &b);

    bool IsCollidingCircleCircle(const Body &a, const Body &b);
}

#endif //PIKUMA_PHYSICS_COLLISIONS_H
