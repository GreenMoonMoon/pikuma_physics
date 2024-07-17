//
// Created by josue on 2023-10-02.
//

#include "shape.h"
#include "external/stb_ds.h"

CircleShape circle_shape_new(const float radius) {
    return (CircleShape) {.radius = radius};
}

float circle_shape_get_moi(const CircleShape shape) {
    return 0.5f * (shape.radius * shape.radius);
};

PolygonShape polygon_shape_new(const float *vertices) {
    PolygonShape shape ={
        .Vertices = vertices,
        .WorldVertices = NULL,
    };
    arrsetlen(shape.WorldVertices, arrlen(vertices));

    return shape;
}

float polygon_shape_get_moi(const PolygonShape shape) {
    return 0;
}

void polygon_shape_update_world_vertices(const PolygonShape shape, const float *position, const float rotation) {
    for(int i = 0; i < arrlen(shape.Vertices); i++){
        // auto transform = glm::identity<glm::mat3x3>();
        // transform = glm::rotate(transform, rotation);
        // transform = glm::translate(transform, position);
        // const glm::vec3 vert = glm::vec3(Vertices[i].x, Vertices[i].y, 1.0f) * transform;
        // WorldVertices[i].x = vert.x;
        // WorldVertices[i].x = vert.y;

        shape.WorldVertices[i][0] = shape.Vertices[i][0] * cosf(rotation) + shape.Vertices[i][1] * sinf(rotation) + position[0];
        shape.WorldVertices[i][1] = shape.Vertices[i][1] * -sinf(rotation) + shape.Vertices[i][1] * cosf(rotation) + position[1];
    }
}

void polygon_shape_find_edge_at(const PolygonShape shape, const int index, vec2 out) {
    int current_vertex = index;
    int next_vertex = (index + 1) % arrlen(shape.WorldVertices);
    glm_vec2_sub(shape.WorldVertices[next_vertex], shape.WorldVertices[index], out);
}

float polygon_shape_find_minimum_separation(const PolygonShape shape, const PolygonShape other, const float *axis, const float *point) {
    float separation = FLT_MIN;

    for (int i = 0; i < arrlen(shape.WorldVertices); i++) {
        vec2 va;
        glm_vec2_copy(shape.WorldVertices[i], va);

        vec2 edge;
        polygon_shape_find_edge_at(shape, i, edge);

        vec2 normal = {edge[1], -edge[0]}; // rotate 90 degrees
        glm_vec2_normalize(normal);

        float min_sep = FLT_MAX;
        for (int j = 0; j < arrlen(other.WorldVertices); ++j) {
            vec2 vb;
            glm_vec2_copy(other.WorldVertices[j], vb);
            vec2 direction;
            glm_vec2_sub(vb, va, direction);
            const float curr_sep = glm_vec2_dot(normal, direction);
            if (curr_sep < min_sep) {
                point = vb;
                min_sep = curr_sep;
            }
        }

        if(min_sep > separation) {
            separation = min_sep;
            axis = edge;
        }
    }

    return separation;
}

BoxShape box_shape_new(const float width, const float height){
    BoxShape shape = {
        .polygon = (PolygonShape){
            .WorldVertices = NULL,
            .Vertices = NULL,
        },
        .width = width,
        .height = height,
    };
    arrsetlen(shape.polygon.Vertices, 4);
    arrsetlen(shape.polygon.WorldVertices, 4);

    glm_vec2_copy((vec2){-0.5f * width, -0.5f * height}, shape.polygon.Vertices[0]);
    glm_vec2_copy((vec2){0.5f * width, -0.5f * height}, shape.polygon.Vertices[1]);
    glm_vec2_copy((vec2){0.5f * width, 0.5f * height}, shape.polygon.Vertices[2]);
    glm_vec2_copy((vec2){-0.5f * width, 0.5f * height}, shape.polygon.Vertices[3]);

    return shape;
}

float box_shape_get_moi(const BoxShape shape) {
    const float ifac = 1.0f/12.0f;
    return ifac * (shape.width * shape.width + shape.height * shape.height);
}
