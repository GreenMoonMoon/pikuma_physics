//
// Created by josue on 2026-01-29.
//

#ifndef PIKUMA_PHYSICS_DEBUG_DRAW_H
#define PIKUMA_PHYSICS_DEBUG_DRAW_H

#include "raylib.h"

void draw_circle_shape(Vector2 position, float radius, float angle, Color color);

void draw_polygon(const Vector2 *points, int point_count, Color color);

void draw_aabs(Vector2 min, Vector2 max, Color color);

#endif //PIKUMA_PHYSICS_DEBUG_DRAW_H