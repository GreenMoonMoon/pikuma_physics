//
// Created by josue on 2024-07-22.
//

#ifndef PIKUMA_PHYSICS_RIGIDBODIES_H
#define PIKUMA_PHYSICS_RIGIDBODIES_H

#include "raylib.h"

typedef enum ShapeType {
    BOX_SHAPE_TYPE,
    POLYGON_SHAPE_TYPE,
    CIRCLE_SHAPE_TYPE,
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

typedef struct Body {
    Vector2 position;
    float rotation;
    Vector2 linear_velocity;
    float angular_velocity;
    float mass;
    float inverse_angular_mass;
    float inverse_mass;
    float restitution;
    ShapeType type;
    union {
        CircleShape circle_shape;
        BoxShape box_shape;
        PolygonShape polygon_shape;
    };
} Body;

Body create_circle_body(float radius, float mass, float restitution, Vector2 position);
Body create_box_body(Vector2 center, Vector2 extents, float mass, float restitution, float position);
Body create_polygon_body(Vector2 *vertices, int vertex_count, float mass, float restitution, float position);

void body_integrate_linear(Body *body, Vector2 force, float delta_time);
void body_integrate_angular(Body *body, float torque, float delta_time);

void body_apply_impulse(Body *body, Vector2 impulse);

#endif //PIKUMA_PHYSICS_RIGIDBODIES_H
