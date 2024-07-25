//
// Created by josue on 2023-10-04.
//

#include "raylib_utils.h"
#include "raylib.h"
#include "rlgl.h"
#include "external/stb_ds.h"

//static Shader gradient_shader;
//
//void Draw::Init() {
//    gradient_shader = LoadShader(nullptr, "../assets/shaders/gradient.frag");
//}
//
//void Draw::Cleanup() {
//    UnloadShader(gradient_shader);
//}
//
//void Draw::ShadedCircle(const Body &body) {
//    float radius = dynamic_cast<CircleShape *>(body.shape.get())->radius;
//    if (body.IsColliding) {
//        DrawCircleGradient(body.Position.x, body.Position.y, radius, RED, DARKBROWN);
//    }
//    else {
//        DrawCircleGradient(body.Position.x, body.Position.y, radius, BLUE, DARKBLUE);
//    }
//}
//
//void Draw::ShadedBox(const Body& body) {
//    auto *shape = dynamic_cast<BoxShape *>(body.shape.get());
//    Rectangle rec = {body.Position.x, body.Position.y, shape->width, shape->height};
//    // float half_width = shape->width * 0.5f;
//    // float half_height = shape->height * 0.5f;
//    if(body.IsColliding) {
//        // DrawRectangleGradientV(body.Position.x - half_width, body.Position.y - half_height, shape->width, shape->height, RED, DARKBROWN);
//        draw_shaded_box(rec, body.Rotation * RAD2DEG,RED, DARKBROWN);
//    }
//    else {
//        // DrawRectangleGradientV(body.Position.x - half_width, body.Position.y - half_height, shape->width, shape->height, BLUE, DARKBLUE);
//        draw_shaded_box(rec, body.Rotation * RAD2DEG,BLUE, DARKBLUE);
//    }
//}
//
//void Draw::Circle(const Body &body) {
//    float radius = dynamic_cast<CircleShape *>(body.shape.get())->radius;
//    float a = glm::mod(body.Rotation * RAD2DEG, 360.0f);
//
//    Color col = body.IsColliding ? RED : WHITE;
//    DrawCircleSectorLines(Vector2(body.Position.x, body.Position.y), radius, a, a + 360.0f, 0, col);
//}
//
//void Draw::Polygon(Body &body) {
//    Color col = body.IsColliding ? RED : WHITE;
//    DrawPolygon(dynamic_cast<PolygonShape *>(body.shape.get())->Vertices, body.Position, body.Rotation, col);
//}

void drawParticle(vec2 position, float radius, ivec4 color) {
    DrawCircle(position[0], position[1], radius, (Color) {color[0], color[1], color[2], color[3]});
}

void drawPolygon(vec2 *vertices, vec2 position, float angle, ivec4 color) {
    rlBegin(RL_LINES);
    rlColor4ub(color[0], color[1], color[2], color[3]);

    vec2 point_a;
    glm_vec2_rotate(arrlast(vertices), -angle, point_a);
    glm_vec2_add(point_a, position, point_a);

    for (int i = 0; i < arrlen(vertices); ++i) {
        vec2 point_b;
        glm_vec2_rotate(vertices[i], -angle, point_b);
        glm_vec2_add(point_b, position, point_b);

        rlVertex2f(point_a[0], point_a[1]);
        rlVertex2f(point_b[0], point_b[1]);

        point_a[0] = point_b[0];
        point_a[1] = point_b[1];
    }

    rlEnd();
}

void draw_shaded_box(const Rectangle rec, const float angle, ivec4 color_a, ivec4 color_b) {
    rlPushMatrix();

    rlBegin(RL_QUADS);


    rlTranslatef(rec.x, rec.y, 0.0f);
    rlRotatef(-angle, 0.0f, 0.0f, 1.0f);
    rlTranslatef(rec.width * -0.5f, rec.height * -0.5f, 0.0f);

    rlNormal3f(0.0f, 0.0f, 1.0f);

    // NOTE: Default raylib font character 95 is a white square
    rlColor4ub(color_a[0], color_a[1], color_a[2], color_a[3]);
    // rlVertex2f(rec.x, rec.y);
    rlVertex2f(0.0f, 0.0f);

    rlColor4ub(color_b[0], color_b[1], color_b[2], color_b[3]);
    // rlVertex2f(rec.x, rec.y + rec.height);
    rlVertex2f(0.0f, rec.height);

    rlColor4ub(color_b[0], color_b[1], color_b[2], color_b[3]);
    // rlVertex2f(rec.x + rec.width, rec.y + rec.height);
    rlVertex2f(rec.width, rec.height);

    rlColor4ub(color_a[0], color_a[1], color_a[2], color_a[3]);
    // rlVertex2f(rec.x + rec.width, rec.y);
    rlVertex2f(rec.width, 0.0f);

    rlEnd();
    rlPopMatrix();
}


void draw_box(vec2 position, float rotation, vec2 center, vec2 extents, ivec4 line_color, ivec4 fill_color) {
    rlPushMatrix();
    rlTranslatef(position[0], position[1], 0.0f);
    rlRotatef(RAD2DEG * rotation, 0.0f, 0.0f, 1.0f);

    rlBegin(RL_QUADS);
    rlColor4ub(fill_color[0], fill_color[1], fill_color[2], fill_color[3]);

    rlVertex2i(center[0] - extents[0], center[1] - extents[1]);
    rlVertex2i(center[0] - extents[0], center[1] + extents[1]);
    rlVertex2i(center[0] + extents[0], center[1] + extents[1]);
    rlVertex2i(center[0] + extents[0], center[1] - extents[1]);

    rlEnd();
    rlBegin(RL_LINES);

    rlColor4ub(line_color[0], line_color[1], line_color[2], line_color[3]);

    rlVertex2i(center[0] - extents[0], center[1] - extents[1]);
    rlVertex2i(center[0] - extents[0], center[1] + extents[1]);

    rlVertex2i(center[0] - extents[0], center[1] + extents[1]);
    rlVertex2i(center[0] + extents[0], center[1] + extents[1]);

    rlVertex2i(center[0] + extents[0], center[1] + extents[1]);
    rlVertex2i(center[0] + extents[0], center[1] - extents[1]);

    rlVertex2i(center[0] + extents[0], center[1] - extents[1]);
    rlVertex2i(center[0] - extents[0], center[1] - extents[1]);

    rlEnd();
    rlPopMatrix();
}

void draw_polygon(vec2 position, float rotation, vec2 *vertices, uint32_t vertex_count, ivec4 line_color, ivec4 fill_color) {
    rlPushMatrix();
    rlTranslatef(position[0], position[1], 0.0f);
    rlRotatef(RAD2DEG * rotation, 0.0f, 0.0f, 1.0f);

    DrawTriangleFan((Vector2 *)vertices, vertex_count, (Color){fill_color[0], fill_color[1], fill_color[2], fill_color[3]});

    rlBegin(RL_LINES);
    rlColor4ub(line_color[0], line_color[1], line_color[2], line_color[3]);

    for (int i = 0; i < vertex_count; ++i) {
        uint32_t n = (i + 1) % vertex_count;
        rlVertex2i(vertices[i][0], vertices[i][1]);
        rlVertex2i(vertices[n][0], vertices[n][1]);
    }

    rlEnd();
    rlPopMatrix();
}

void draw_circle(vec2 position, float rotation, float radius, ivec4 line_color, ivec4 fill_color) {
    DrawCircle(position[0], position[1], radius, (Color){fill_color[0], fill_color[1], fill_color[2], fill_color[3]});
    DrawCircleLines(position[0], position[1], radius, (Color) {line_color[0], line_color[1], line_color[2], line_color[3]});
    vec2 rotated_line;
    glm_vec2_rotate((vec2){radius, 0.0f}, rotation, rotated_line);
    DrawLine(position[0], position[1], position[0] + rotated_line[0], position[1] + rotated_line[1], (Color) {line_color[0], line_color[1], line_color[2], line_color[3]});
}

void draw_grid(int spacing) {
    rlPushMatrix();

    rlBegin(RL_LINES);
    rlColor4f(0.9f, 0.9f, 0.9f, 1.0f);

    for (int i = 0; (i * spacing) < GetScreenHeight(); ++i) {
        rlVertex2i(0, i * spacing);
        rlVertex2i(GetScreenWidth(), i * spacing);
    }

    for (int i = 0; (i * spacing) < GetScreenWidth(); ++i) {
        rlVertex2i(i * spacing, 0);
        rlVertex2i(i * spacing, GetScreenHeight());
    }

    rlEnd();
    rlPopMatrix();
}
