//
// Created by josue on 2024-07-22.
//

#include "rigidbodies.h"

#include <float.h>

#include "raymath.h"

float calculate_circle_angular_mass(float radius, float mass){
    return 0.5f * (radius * radius) * mass;
}

float calculate_square_angular_mass(const Vector2 extents, const float mass) {
    return 12.0f * ((extents.x * 2) * (extents.x * 2) + (2 * extents.y) * (2 * extents.y)) * mass;
}

float calculate_polygon_angular_mass(float mass){
    return 1.0f;
}

Body create_circle_body(const float radius, const float mass, const float restitution, const Vector2 position) {
    const Body result = {
            .position = position,
            .rotation = 0.0f,
            .linear_velocity = {0},
            .angular_velocity = 0.0f,
            .inverse_mass = 1.0f / mass,
            .mass = mass,
            .inverse_angular_mass = calculate_circle_angular_mass(radius, mass),
            .restitution = restitution,
            .type = CIRCLE_SHAPE_TYPE,
            .circle_shape = (CircleShape){.radius = radius},
    };
    return result;
}

Body create_box_body(const Vector2 center, const Vector2 extents, const float mass, const float restitution, const Vector2 position) {
    Body result ={
            .position = position,
            .rotation = 0.0f,
            .linear_velocity = {0},
            .angular_velocity = 0.0f,
            .inverse_mass = 1.0f / mass,
            .mass = mass,
            .inverse_angular_mass = calculate_square_angular_mass(extents, mass),
            .restitution = restitution,
            .type = BOX_SHAPE_TYPE,
            .box_shape = {0},
    };
    result.box_shape.center = center;
    result.box_shape.extents = extents;
    return result;
}

Body create_polygon_body(Vector2 *vertices, const int vertex_count, const float mass, const float restitution, const Vector2 position) {
    const Body result = {
        .position = position,
        .rotation = 0.0f,
        .linear_velocity = {0},
        .angular_velocity = 0.0f,
        .inverse_mass = 1.0f / mass,
        .mass = mass,
        .restitution = restitution,
        .inverse_angular_mass = calculate_polygon_angular_mass(mass),
        .type = POLYGON_SHAPE_TYPE,
        .polygon_shape = (PolygonShape){
            .vertices = vertices,
            .vertex_count = vertex_count,
        }
    };
    return result;
}

void body_integrate_linear(Body *body, const Vector2 force, float delta_time) {
    const Vector2 acceleration = Vector2Scale(force, delta_time);
    body->linear_velocity = Vector2Add(body->linear_velocity, acceleration);

    Vector2 delta_velocity = Vector2Scale(body->linear_velocity, delta_time);
    body->position = Vector2Add(body->position, delta_velocity);
}

void body_integrate_angular(Body *body, const float torque, const float delta_time) {
    body->angular_velocity += torque * delta_time;
    body->rotation += body->angular_velocity * delta_time;
}

void body_apply_impulse(Body *body, const Vector2 impulse) {
    /* An impulse is an instant change in velocity inversely proportional to the mass of the object.
     the momentum is the mass times the velocity: P=m*v
     An impulse is the change in that momentum: J = deltaP = m*deltaV (mass doesn't change)
    so j/m = deltaV.
    */
    Vector2 j = Vector2Scale(impulse, body->inverse_mass);
    body->linear_velocity = Vector2Add(body->linear_velocity, j);
}

BoundingSquare get_polygon_bounding_square(const PolygonShape polygon_shape, const Vector2 position, const float angle) {
    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float max_x = -FLT_MAX;
    float max_y = -FLT_MAX;
    for (int i = 0; i < polygon_shape.vertex_count; ++i) {
        const Vector2 point = Vector2Rotate(polygon_shape.vertices[i], angle);
        if (point.x < min_x) { min_x = point.x; }
        if (point.x > max_x) { max_x = point.x; }
        if (point.y < min_y) { min_y = point.y; }
        if (point.y > max_y) { max_y = point.y; }
    }

    return (BoundingSquare){
        .min = {.x = min_x + position.x, .y = min_y + position.y},
        .max = {.x = max_y + position.x, .y = max_y + position.y}
    };
}
