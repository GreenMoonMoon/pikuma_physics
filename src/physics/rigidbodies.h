//
// Created by josue on 2024-07-22.
//

#ifndef PIKUMA_PHYSICS_RIGIDBODIES_H
#define PIKUMA_PHYSICS_RIGIDBODIES_H

#include "cglm/cglm.h"

typedef enum ShapeType {
    BOX_SHAPE_TYPE,
    POLYGON_SHAPE_TYPE,
    CIRCLE_SHAPE_TYPE,
} ShapeType;

typedef struct BoxShape {
    vec2 center;
    vec2 extents;
} BoxShape;

typedef struct PolygonShape {
    vec2 *vertices;
    uint32_t vertex_count;
} PolygonShape;

typedef struct CircleShape {
    float radius;
} CircleShape;

typedef struct Body {
    vec2 position;
    float rotation;
    vec2 linear_velocity;
    float angular_velocity;
    float mass;
    float inverse_angular_mass;
    float inverse_mass;
    ShapeType type;
    union {
        CircleShape circle_shape;
        BoxShape box_shape;
        PolygonShape polygon_shape;
    };
} Body;

Body create_circle_body(float radius, float mass, vec2 position);
Body create_box_body(vec2 center, vec2 extents, float mass, vec2 position);
Body create_polygon_body(vec2 *vertices, uint32_t vertex_count, float mass, vec2 position);

void circle_check_resolve_boundary(Body *body, const vec2 min, const vec2 max);

void body_integrate_linear(Body *body, vec2 force, float delta_time);
void body_integrate_angular(Body *body, float torque, float delta_time);

#endif //PIKUMA_PHYSICS_RIGIDBODIES_H
