//
// Created by josue on 2024-07-22.
//

#ifndef PIKUMA_PHYSICS_RIGIDBODIES_H
#define PIKUMA_PHYSICS_RIGIDBODIES_H

#include "raylib.h"
#include "physic_types.h"

typedef struct Body {
    Vector2 position;
    Vector2 linear_velocity;
    // bounding square for broad-phase collisions
    BoundingSquare bounding_square;
    float rotation;
    float angular_velocity;
    float mass;
    float inverse_angular_mass;
    float inverse_mass;
    float restitution;
    ShapeType type;
    union {
        CircleShape circle_shape;
        PolygonShape polygon_shape;
    };

    // DEBUG
    bool is_colliding;
    bool aabs_is_overlapping;
} Body;

Body create_circle_body(float radius, float mass, float restitution, Vector2 position);

Body create_box_body(Vector2 center, Vector2 extents, float mass, float restitution, Vector2 position);

Body create_polygon_body(const Vector2 *vertices, int vertex_count, float mass, float restitution, Vector2 position);

void free_polygon_shape(PolygonShape polygon_shape);

void body_integrate_linear(Body *body, Vector2 force, float delta_time);

void body_integrate_angular(Body *body, float torque, float delta_time);

void body_apply_impulse(Body *body, Vector2 impulse);

void update_polygon_shape(PolygonShape *shape, Vector2 position, float angle);

BoundingSquare get_polygon_bounding_square(PolygonShape polygon_shape, Vector2 position, float angle);

#endif //PIKUMA_PHYSICS_RIGIDBODIES_H
