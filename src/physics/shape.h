//
// Created by josue on 2023-10-02.
//

#ifndef PIKUMA_PHYSICS_SHAPE_H
#define PIKUMA_PHYSICS_SHAPE_H

#include "cglm/cglm.h"

typedef enum ShapeType {
    CIRCLE,
    POLYGON,
    BOX,
} ShapeType;

typedef struct CircleShape {
    float radius;
} CircleShape;

typedef struct PolygonShape {
    vec2 *Vertices = NULL;
    vec2 *WorldVertices = NULL;
} PolygonShape;

typedef struct BoxShape {
    PolygonShape polygon;
    float width;
    float height;
} BoxShape;

typedef struct Shape {
    ShapeType type;
    union {
        CircleShape circle = {0};
        BoxShape box;
        PolygonShape polygon;
    };
} Shape;

CircleShape circle_shape_new(float radius);

float circle_shape_get_moi(CircleShape shape);

PolygonShape polygon_shape_new(const float *vertices);

float polygon_shape_get_moi(PolygonShape shape);

void polygon_shape_update_world_vertices(PolygonShape shape, const float *position,  float rotation);

void polygon_shape_find_edge_at(PolygonShape shape, int index, vec2 out);

float polygon_shape_find_minimum_separation(PolygonShape shape, PolygonShape other, const float *axis, const float *point);

BoxShape box_shape_new(float width, float height);

float box_shape_get_moi(BoxShape shape);

#endif //PIKUMA_PHYSICS_SHAPE_H
