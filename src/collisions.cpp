//
// Created by josue on 2023-10-10.
//

#include "collisions.h"
#include "glm/vec2.hpp"
#include "glm/geometric.hpp"

using glm::vec2;

bool CollisionDetection::IsColliding(const Body &a, const Body &b, Contact &contact) {
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

bool CollisionDetection::IsCollidingCircleCircle(const Body &a, const Body &b, Contact &contact) {
    Shape *shapeA = a.shape.get();
    Shape *shapeB = b.shape.get();
    const vec2 direction = b.Position - a.Position;

    const float radiusSum = dynamic_cast<CircleShape *>(shapeA)->radius + dynamic_cast<CircleShape *>(shapeB)->radius;

    if (glm::length(direction) < radiusSum) {
        return true;
    }

    return false;
}
