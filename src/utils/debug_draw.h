//
// Created by josue on 2026-01-29.
//

#ifndef PIKUMA_PHYSICS_DEBUG_DRAW_H
#define PIKUMA_PHYSICS_DEBUG_DRAW_H

#include "raylib.h"

void draw_collision(Vector2 point, Vector2 normal, Color color);

void draw_circle_shape(Vector2 position, float radius, float angle, Color color);

#endif //PIKUMA_PHYSICS_DEBUG_DRAW_H