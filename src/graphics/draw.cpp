//
// Created by josue on 2023-10-17.
//

#include "draw.h"
#include "physics/shape.h"
#include "raylib.h"
#include "raylib_extension.h"
#include "glm/ext/scalar_common.hpp"

static Shader gradient_shader;

void Draw::Init() {
    gradient_shader = LoadShader(nullptr, "../assets/shaders/gradient.frag");
}

void Draw::Cleanup() {
    UnloadShader(gradient_shader);
}

void Draw::ShadedCircle(const Body &body) {
    float radius = dynamic_cast<CircleShape *>(body.shape.get())->radius;
    if (body.IsColliding) {
        DrawCircleGradient(body.Position.x, body.Position.y, radius, RED, DARKBROWN);
    }
    else {
        DrawCircleGradient(body.Position.x, body.Position.y, radius, BLUE, DARKBLUE);
    }
}

void Draw::ShadedBox(const Body& body) {
    auto *shape = dynamic_cast<BoxShape *>(body.shape.get());
    Rectangle rec = {body.Position.x, body.Position.y, shape->width, shape->height};
    // float half_width = shape->width * 0.5f;
    // float half_height = shape->height * 0.5f;
    if(body.IsColliding) {
        // DrawRectangleGradientV(body.Position.x - half_width, body.Position.y - half_height, shape->width, shape->height, RED, DARKBROWN);
        draw_shaded_box(rec, body.Rotation * RAD2DEG,RED, DARKBROWN);
    }
    else {
        // DrawRectangleGradientV(body.Position.x - half_width, body.Position.y - half_height, shape->width, shape->height, BLUE, DARKBLUE);
        draw_shaded_box(rec, body.Rotation * RAD2DEG,BLUE, DARKBLUE);
    }
}

void Draw::Circle(const Body &body) {
    float radius = dynamic_cast<CircleShape *>(body.shape.get())->radius;
    float a = glm::mod(body.Rotation * RAD2DEG, 360.0f);

    Color col = body.IsColliding ? RED : WHITE;
    DrawCircleSectorLines(Vector2(body.Position.x, body.Position.y), radius, a, a + 360.0f, 0, col);
}

void Draw::Polygon(Body &body) {
    Color col = body.IsColliding ? RED : WHITE;
    DrawPolygon(dynamic_cast<PolygonShape *>(body.shape.get())->Vertices, body.Position, body.Rotation, col);
}