//
// Created by josue on 2024-07-22.
//

#include <stdlib.h>
#include <string.h>
#include <float.h>
#include "rigidbodies.h"
#include "raymath.h"

float calculate_circle_angular_mass(const float radius){
    return 0.5f * (radius * radius);
}

float calculate_square_angular_mass(const Vector2 extents) {
    // return 1/12 * ((extents.x * 2) * (extents.x * 2) + (2 * extents.y) * (2 * extents.y)) * mass;
    return (0.083333f) * (extents.x * extents.x + extents.y * extents.y);
}

float calculate_polygon_angular_mass(){
    return 1.0f;
}

Body create_circle_body(const float radius, const float mass, const float restitution, const Vector2 position) {
    float inverse_mass = 0.0f;
    if (!FloatEquals(mass, 0.0f)) {
        inverse_mass = 1.0f / mass;
    };
    const float angular_mass = calculate_circle_angular_mass(radius) * mass;
    float inverse_angular_mass = 0.0f;
    if (!FloatEquals(angular_mass, 0.0f)) {
        inverse_angular_mass = 1.0f / angular_mass;
    }
    const Body result = {
        .position = position,
        .rotation = 0.0f,
        .linear_velocity = {0},
        .angular_velocity = 0.0f,
        .inverse_mass = inverse_mass,
        .mass = mass,
        .angular_mass = angular_mass,
        .inverse_angular_mass = inverse_angular_mass,
        .restitution = restitution,
        .type = CIRCLE_SHAPE_TYPE,
        .circle_shape = (CircleShape){.radius = radius},
        .bounding_square = {
            .min = Vector2Subtract(position, (Vector2){radius, radius}),
            .max = Vector2Add(position, (Vector2){radius, radius}),
        },
        .is_colliding = false,
        .aabs_is_overlapping = false
    };
    return result;
}

Body create_box_body(const Vector2 center, const Vector2 extents, const float mass, const float restitution, const Vector2 position, const bool is_static) {
    float inverse_mass = 0.0f;
    if (!FloatEquals(mass, 0.0f)) {
        inverse_mass = 1.0f / mass;
    };
    const float angular_mass = calculate_square_angular_mass(extents) * mass;
    float inverse_angular_mass = 0.0f;
    if (!FloatEquals(angular_mass, 0.0f)) {
        inverse_angular_mass = 1.0f / angular_mass;
    }
    Body result ={
        .position = position,
        .rotation = 0.0f,
        .linear_velocity = {0},
        .angular_velocity = 0.0f,
        .inverse_mass = inverse_mass,
        .mass = mass,
        .angular_mass = angular_mass,
        .inverse_angular_mass = inverse_angular_mass,
        .restitution = restitution,
        // .is_static = mass < EPSILON,
        .is_static = is_static,
        .type = POLYGON_SHAPE_TYPE,
        .polygon_shape = {0},
        .bounding_square = {0},
        .is_colliding = false,
        .aabs_is_overlapping = false
    };
    result.polygon_shape.vertices = malloc(sizeof(Vector2) * 8);
    result.polygon_shape.tfmd_vertices = &result.polygon_shape.vertices[4];
    if (result.polygon_shape.vertices != NULL) {
        result.polygon_shape.vertex_count = 4;
        result.polygon_shape.vertices[0] = (Vector2){center.x + extents.x, center.y + extents.y};
        result.polygon_shape.vertices[1] = (Vector2){center.x - extents.y, center.y + extents.y};
        result.polygon_shape.vertices[2] = (Vector2){center.x - extents.y, center.y - extents.y};
        result.polygon_shape.vertices[3] = (Vector2){center.x + extents.x, center.y - extents.y};

        update_polygon_shape(&result.polygon_shape, result.position, result.rotation);
    }

    return result;
}

Body create_polygon_body(const Vector2 *vertices, const int vertex_count, const float mass, const float restitution, const Vector2 position) {
    float inverse_mass = 0.0f;
    if (!FloatEquals(mass, 0.0f)) {
        inverse_mass = 1.0f / mass;
    };
    const float angular_mass = calculate_polygon_angular_mass() * mass;
    float inverse_angular_mass = 0.0f;
    if (!FloatEquals(angular_mass, 0.0f)) {
        inverse_angular_mass = 1.0f / angular_mass;
    }
    Body result = {
        .position = position,
        .rotation = 0.0f,
        .linear_velocity = {0},
        .angular_velocity = 0.0f,
        .inverse_mass = inverse_mass,
        .mass = mass,
        .restitution = restitution,
        .angular_mass = angular_mass,
        .inverse_angular_mass = inverse_angular_mass,
        .is_static = mass < EPSILON,
        .type = POLYGON_SHAPE_TYPE,
        .polygon_shape = (PolygonShape){
            .vertices = malloc(sizeof(Vector2) * vertex_count * 2),
            .vertex_count = vertex_count,
        },
        .bounding_square = {0},
        .is_colliding = false,
        .aabs_is_overlapping = false
    };
    result.polygon_shape.tfmd_vertices = &result.polygon_shape.vertices[vertex_count];
    memcpy(result.polygon_shape.vertices, vertices, sizeof(Vector2) * vertex_count);
    update_polygon_shape(&result.polygon_shape, result.position, result.rotation);

    return result;
}

void free_polygon_shape(PolygonShape polygon_shape) {
    if (polygon_shape.vertices != NULL) {
        free(polygon_shape.vertices);
        polygon_shape.vertices = NULL;
        polygon_shape.vertex_count = 0;
    }
}

void body_integrate_linear(Body *body, const Vector2 force, float delta_time) {
    if (body->is_static) { return; }
    const Vector2 acceleration = Vector2Scale(force, delta_time);
    body->linear_velocity = Vector2Add(body->linear_velocity, acceleration);

    Vector2 delta_velocity = Vector2Scale(body->linear_velocity, delta_time);
    body->position = Vector2Add(body->position, delta_velocity);
}

void body_integrate_angular(Body *body, const float torque, const float delta_time) {
    body->angular_velocity += torque * delta_time;
    body->rotation += body->angular_velocity * delta_time;
}

void body_apply_impulse(Body* body, const Vector2 impulse, const Vector2 r) {
    /* Apply both linear and angular impulse. Linear impulse is applied to the center of mass
     * while the angular impulse is applied to the point of contact denoted by the r vector. */

    if (body->is_static) { return; }

    // apply linear impulse
    const Vector2 j = Vector2Scale(impulse, body->inverse_mass);
    body->linear_velocity = Vector2Add(body->linear_velocity, j);

    // apply angular impulse ( j X r)
    body->angular_velocity += (j.x * r.y - j.y * r.y) * body->inverse_angular_mass;
}

void body_apply_linear_impulse(Body *body, const Vector2 impulse) {
    /* An impulse is an instant change in velocity inversely proportional to the mass of the object.
     * the momentum is the mass times the velocity: P=m*v
     * An impulse is the change in that momentum: J = deltaP = m*deltaV (mass doesn't change)
     * so j/m = deltaV. */
    if (body->is_static) { return; }
    const Vector2 j = Vector2Scale(impulse, body->inverse_mass);
    body->linear_velocity = Vector2Add(body->linear_velocity, j);
}

void update_polygon_shape(PolygonShape *shape, const Vector2 position, const float angle) {
    for (int i = 0; i < shape->vertex_count; ++i) {
        shape->tfmd_vertices[i] = Vector2Add(Vector2Rotate(shape->vertices[i], angle), position);
    }
}

BoundingSquare get_polygon_bounding_square(PolygonShape polygon_shape, const Vector2 position, const float angle) {
    update_polygon_shape(&polygon_shape, position, angle);
    float min_x = FLT_MAX;
    float min_y = FLT_MAX;
    float max_x = -FLT_MAX;
    float max_y = -FLT_MAX;
    for (int i = 0; i < polygon_shape.vertex_count; ++i) {
        if (polygon_shape.tfmd_vertices[i].x < min_x) { min_x = polygon_shape.tfmd_vertices[i].x; }
        if (polygon_shape.tfmd_vertices[i].x > max_x) { max_x = polygon_shape.tfmd_vertices[i].x; }
        if (polygon_shape.tfmd_vertices[i].y < min_y) { min_y = polygon_shape.tfmd_vertices[i].y; }
        if (polygon_shape.tfmd_vertices[i].y > max_y) { max_y = polygon_shape.tfmd_vertices[i].y; }
    }

    return (BoundingSquare){
        .min = {.x = min_x, .y = min_y},
        .max = {.x = max_x, .y = max_y}
    };
}
