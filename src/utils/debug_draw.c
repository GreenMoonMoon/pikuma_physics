//
// Created by josue on 2026-01-29.
//

#include "debug_draw.h"
#include "rlgl.h"
#include <math.h>

#include "raymath.h"

void draw_circle_shape(const Vector2 position, const float radius, const float angle, const Color color) {
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    for (int i = 0; i < 360; i += 10) {
        rlVertex2f(position.x + cosf(DEG2RAD*i)*radius, position.y + sinf(DEG2RAD*i)*radius);
        rlVertex2f(position.x + cosf(DEG2RAD*(i + 10))*radius, position.y + sinf(DEG2RAD*(i + 10))*radius);
    }

    rlVertex2f(position.x, position.y);
    rlVertex2f(position.x + cosf(angle) * radius, position.y + sinf(angle) * radius);
    rlEnd();
}

void draw_polygon(const Vector2 *points, const int point_count, const Color color) {
    if (point_count < 2) return; // Security check

    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    for (int i = 0; i < point_count; i++) {
        const int j = (i + 1) % point_count;

        // calculate normal
        const Vector2 edge = Vector2Subtract(points[j], points[i]);
        const Vector2 edge_n = Vector2Normalize(edge);
        const Vector2 normal = {edge_n.y, -edge_n.x};
        const Vector2 start = Vector2Add(points[i], Vector2Scale(edge, 0.5f));

        rlVertex2f(points[i].x, points[i].y);
        rlVertex2f(points[j].x, points[j].y);

        rlVertex2f(start.x, start.y);
        rlVertex2f(start.x + normal.x * 20.0f, start.y + normal.y * 20.0f);
    }

    rlEnd();
}

void draw_aabs(Vector2 min, Vector2 max, Color color) {
    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    rlVertex2f(min.x, min.y);
    rlVertex2f(min.x, max.y);

    rlVertex2f(min.x, max.y);
    rlVertex2f(max.x, max.y);

    rlVertex2f(max.x, max.y);
    rlVertex2f(max.x, min.y);

    rlVertex2f(max.x, min.y);
    rlVertex2f(min.x, min.y);

    rlEnd();
}
