//
// Created by josue on 2023-10-17.
//

#ifndef PIKUMA_PHYSICS_DRAW_H
#define PIKUMA_PHYSICS_DRAW_H

#endif //PIKUMA_PHYSICS_DRAW_H

#include "physics/body.h"

namespace Draw {
    void Init();
    void Cleanup();

    void ShadedCircle(const Body &body);
    void ShadedBox(const Body &body);

    void Circle(const Body &body);
    void Polygon(const Body &body);
}