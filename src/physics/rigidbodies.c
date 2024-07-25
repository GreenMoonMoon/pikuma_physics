//
// Created by josue on 2024-07-22.
//

#include "rigidbodies.h"

Body create_circle_body(float radius, float mass, vec2 position) {
    Body result = {
            .position = {position[0], position[1]},
            .rotation = 0.0f,
            .velocity = {0},
            .inverse_mass = 1.0f / mass,
            .mass = mass,
            .type = CIRCLE_SHAPE_TYPE,
            .circle_shape = (CircleShape){.radius = radius},
    };
    return result;
}

Body create_box_body(vec2 center, vec2 extents, float mass, float *position) {
    Body result ={
            .position = {position[0], position[1]},
            .rotation = 0.0f,
            .velocity = {0},
            .inverse_mass = 1.0f / mass,
            .mass = mass,
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
        .velocity = {0},
        .inverse_mass = 1.0f / mass,
        .mass = mass,
        .type = POLYGON_SHAPE_TYPE,
        .polygon_shape = (PolygonShape){
            .vertices = vertices,
            .vertex_count = vertex_count,
            },
    };
    return result;
}


