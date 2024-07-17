//
// Created by josue on 2023-10-04.
//

#ifndef PIKUMA_PHYSICS_RAYLIB_UTILS_H
#define PIKUMA_PHYSICS_RAYLIB_UTILS_H

#include "cglm/cglm.h"

//void Init();
//void Cleanup();
//
//void ShadedCircle(const Body &body);
//void ShadedBox(const Body &body);
//
//void Circle(const Body &body);
//void drawPolygonBody(Body &body);

void drawParticle(vec2 position, float radius, ivec4 color);
void drawPolygon(vec2 *vertices, vec2 position, float angle, ivec4 color);
void drawShadedBox(vec2 min, vec2 max, float angle, ivec4 color_a, ivec4 color_b);

#endif //PIKUMA_PHYSICS_RAYLIB_UTILS_H
