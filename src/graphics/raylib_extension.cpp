//
// Created by josue on 2023-10-04.
//

#include "raylib_extension.h"
#include "rlgl.h"
#define GLM_ENABLE_EXPERIMENTAL
#include "glm/gtx/rotate_vector.hpp"


void DrawPolygon(const std::vector<glm::vec2> &vertices, const glm::vec2 position, const float angle, const Color color) {
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    glm::vec2 pointA = glm::rotate(vertices.back(), -angle) + position;

    for (auto vertex : vertices) {
        glm::vec2 pointB = glm::rotate(vertex, -angle) + position;

        rlVertex2f(pointA.x, pointA.y);
        rlVertex2f(pointB.x, pointB.y);

        pointA = pointB;
    }

    rlEnd();
}

void draw_shaded_box(const Rectangle rec, const float angle, const Color a, const Color b) {
    rlPushMatrix();

    rlBegin(RL_QUADS);


    rlTranslatef(rec.x, rec.y, 0.0f);
    rlRotatef(-angle, 0.0f, 0.0f, 1.0f);
    rlTranslatef(rec.width * -0.5f, rec.height * -0.5f, 0.0f);

    rlNormal3f(0.0f, 0.0f, 1.0f);

    // NOTE: Default raylib font character 95 is a white square
    rlColor4ub(a.r, a.g, a.b, a.a);
    // rlVertex2f(rec.x, rec.y);
    rlVertex2f(0.0f, 0.0f);

    rlColor4ub(b.r, b.g, b.b, b.a);
    // rlVertex2f(rec.x, rec.y + rec.height);
    rlVertex2f(0.0f, rec.height);

    rlColor4ub(b.r, b.g, b.b, b.a);
    // rlVertex2f(rec.x + rec.width, rec.y + rec.height);
    rlVertex2f(rec.width, rec.height);

    rlColor4ub(a.r, a.g, a.b, a.a);
    // rlVertex2f(rec.x + rec.width, rec.y);
    rlVertex2f(rec.width, 0.0f);

    rlEnd();
    rlPopMatrix();
}
