//
// Created by josue on 2026-01-29.
//

#ifndef PIKUMA_PHYSICS_TYPES_H
#define PIKUMA_PHYSICS_TYPES_H

#include "raylib.h"

typedef enum ShapeType {
    POLYGON_SHAPE_TYPE,
    CIRCLE_SHAPE_TYPE,
    BOX_SHAPE_TYPE,
} ShapeType;

typedef struct BoxShape {
    Vector2 center;
    Vector2 extents;
} BoxShape;

typedef struct PolygonShape {
    Vector2 *vertices;
    int vertex_count;
} PolygonShape;

typedef struct CircleShape {
    float radius;
} CircleShape;

typedef struct BoundingSquare {
    Vector2 min;
    Vector2 max;
} BoundingSquare;

#endif //PIKUMA_PHYSICS_TYPES_H