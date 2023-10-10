//
// Created by josue on 2023-10-04.
//

#include "raylib_extension.h"
#include "rlgl.h"
#include "glm/gtx/rotate_vector.hpp"

void DrawPolygon(std::vector<glm::vec2> &vertices, glm::vec2 position, float angle, Color color) {
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
