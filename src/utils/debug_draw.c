//
// Created by josue on 2026-01-29.
//

#include "debug_draw.h"
#include "rlgl.h"
#include <math.h>

void draw_collision(const Vector2 point, const Vector2 normal, const Color color) {
    const Texture2D tex_shapes = GetShapesTexture();

    const Vector2 topLeft = { point.x - 4, point.y - 4 };
    const Vector2 topRight = { point.x + 4, point.y - 4 };
    const Vector2 bottomLeft = { point.x - 4, point.y + 4 };
    const Vector2 bottomRight = { point.x + 4, point.y + 4 };

    rlSetTexture(GetShapesTexture().id);
    const Rectangle shape_rect = GetShapesTextureRectangle();
    rlBegin(RL_QUADS);

    rlNormal3f(0.0f, 0.0f, 1.0f);
    rlColor4ub(color.r, color.g, color.b, color.a);

    rlTexCoord2f(shape_rect.x/tex_shapes.width, shape_rect.y/tex_shapes.height);
    rlVertex2f(topLeft.x, topLeft.y);

    rlTexCoord2f(shape_rect.x/tex_shapes.width, (shape_rect.y + shape_rect.height)/tex_shapes.height);
    rlVertex2f(bottomLeft.x, bottomLeft.y);

    rlTexCoord2f((shape_rect.x + shape_rect.width)/tex_shapes.width, (shape_rect.y + shape_rect.height)/tex_shapes.height);
    rlVertex2f(bottomRight.x, bottomRight.y);

    rlTexCoord2f((shape_rect.x + shape_rect.width)/tex_shapes.width, shape_rect.y/tex_shapes.height);
    rlVertex2f(topRight.x, topRight.y);

    rlEnd();

    rlSetTexture(0);

    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);
    rlVertex2f(point.x, point.y);
    rlVertex2f(point.x + normal.x * 15.0f, point.y + normal.y * 15.0f);
    rlEnd();
}

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

void draw_polygon(const Vector2 position, const Vector2 *points, const int point_count, const Color color) {
    if (point_count < 2) return; // Security check

    rlBegin(RL_LINES);
    rlColor4ub(color.r, color.g, color.b, color.a);

    for (int i = 0; i < point_count; i++) {
        int j = (i + 1) % point_count;
        rlVertex2f(points[i].x + position.x, points[i].y + position.y);
        rlVertex2f(points[j].x + position.x, points[j].y + position.y);
    }

    rlEnd();
}
