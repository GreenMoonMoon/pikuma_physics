//
// Created by josue on 2024-07-22.
//

#include "rigidbodies.h"

float calculate_circle_angular_mass(float radius, float mass){
    return 0.5f * (radius * radius) * mass;
}

float calculate_square_angular_mass(const vec2 extents, float mass) {
    return 12.0f * (glm_pow2(2 * extents[0]) + glm_pow2(2 * extents[1])) * mass;
}

float calculate_polygon_angular_mass(float mass){
    return 1.0f;
}

Body create_circle_body(float radius, float mass, vec2 position) {
    Body result = {
            .position = {position[0], position[1]},
            .rotation = 0.0f,
            .linear_velocity = {0},
            .angular_velocity = 0.0f,
            .inverse_mass = 1.0f / mass,
            .mass = mass,
            .inverse_angular_mass = calculate_circle_angular_mass(radius, mass),
            .type = CIRCLE_SHAPE_TYPE,
            .circle_shape = (CircleShape){.radius = radius},
    };
    return result;
}

Body create_box_body(vec2 center, vec2 extents, float mass, float *position) {
    Body result ={
            .position = {position[0], position[1]},
            .rotation = 0.0f,
            .linear_velocity = {0},
            .angular_velocity = 0.0f,
            .inverse_mass = 1.0f / mass,
            .mass = mass,
            .inverse_angular_mass = calculate_square_angular_mass(extents, mass),
            .type = BOX_SHAPE_TYPE,
            .box_shape = {0},
    };
    glm_vec2_copy(center, result.box_shape.center);
    glm_vec2_copy(extents, result.box_shape.extents);
    return result;
}

Body create_polygon_body(vec2 *vertices, uint32_t vertex_count, float mass, float *position) {
    Body result = {
        .position = {position[0], position[1]},
        .rotation = 0.0f,
        .linear_velocity = {0},
        .angular_velocity = 0.0f,
        .inverse_mass = 1.0f / mass,
        .mass = mass,
        .inverse_angular_mass = calculate_polygon_angular_mass(mass),
        .type = POLYGON_SHAPE_TYPE,
        .polygon_shape = (PolygonShape){
            .vertices = vertices,
            .vertex_count = vertex_count,
            },
    };
    return result;
}

void circle_check_resolve_boundary(Body *body, const vec2 min, const vec2 max) {
    if (body->position[0]-body->circle_shape.radius < min[0]) {
        body->position[0] = body->circle_shape.radius + min[0];
        body->linear_velocity[0] = -body->linear_velocity[0];
    } else if (body->position[0]+body->circle_shape.radius > max[0]){
        body->position[0] = max[0] - body->circle_shape.radius;
        body->linear_velocity[0] = -body->linear_velocity[0];
    }
    if (body->position[1]-body->circle_shape.radius < min[1]) {
        body->position[1] = body->circle_shape.radius + min[1];
        body->linear_velocity[1] = -body->linear_velocity[1];
    } else if (body->position[1]+body->circle_shape.radius > max[1]){
        body->position[1] = max[1] - body->circle_shape.radius;
        body->linear_velocity[1] = -body->linear_velocity[1];
    }
}

void body_integrate_linear(Body *body, vec2 force, float delta_time) {
    vec2 acceleration;
    glm_vec2_scale(force, delta_time, acceleration);
    glm_vec2_add(body->linear_velocity, acceleration, body->linear_velocity);

    vec2 delta_velocity;
    glm_vec2_scale(body->linear_velocity, delta_time, delta_velocity);
    glm_vec2_add(body->position, delta_velocity, body->position);
}

void body_integrate_angular(Body *body, float torque, float delta_time) {
    body->angular_velocity += torque * delta_time;
    body->rotation += body->angular_velocity * delta_time;
}
