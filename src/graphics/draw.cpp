//
// Created by josue on 2023-10-17.
//

#include "draw.h"
#include "physics/shape.h"
#include "raylib.h"
#include "raylib_extension.h"
#include "glm/ext/scalar_common.hpp"

static Shader gradient_shader;
static Texture test_texture;

void Draw::Init() {
    gradient_shader = LoadShader(nullptr, "../assets/shaders/gradient.frag");
    test_texture = LoadTexture("../assets/spritesheet/spritesheet-tiles-default.png");
}

void Draw::Cleanup() {
    UnloadTexture(test_texture);
    UnloadShader(gradient_shader);
}

void Draw::ShadedCircle(const Body &body) {
    const float radius = dynamic_cast<CircleShape *>(body.shape.get())->Radius;
    if (body.IsColliding) {
        DrawCircleGradient(body.Position.x, body.Position.y, radius, RED, DARKBROWN);
    }
    else {
        DrawCircleGradient(body.Position.x, body.Position.y, radius, BLUE, DARKBLUE);
    }
}

void Draw::ShadedBox(const Body& body) {
    const auto *shape = dynamic_cast<BoxShape *>(body.shape.get());
    // const Rectangle rec = {body.Position.x, body.Position.y, shape->width, shape->height};
    const float half_width = shape->width * 0.5f;
    // // float half_height = shape->height * 0.5f;
    // if(body.IsColliding) {
    //     // DrawRectangleGradientV(body.Position.x - half_width, body.Position.y - half_height, shape->width, shape->height, RED, DARKBROWN);
    //     draw_shaded_box(rec, body.Rotation * RAD2DEG,RED, DARKBROWN);
    // }
    // else {
    //     // DrawRectangleGradientV(body.Position.x - half_width, body.Position.y - half_height, shape->width, shape->height, BLUE, DARKBLUE);
    //     draw_shaded_box(rec, body.Rotation * RAD2DEG,BLUE, DARKBLUE);
    // }
    if (body.IsColliding) {
        DrawTexturePro(test_texture, (Rectangle){0, 0, 64.0f, 64.0f}, (Rectangle){body.Position.x, body.Position.y, shape->width, shape->width}, (Vector2){half_width, half_width}, -body.Rotation * RAD2DEG, RED);
    } else {
        DrawTexturePro(test_texture, (Rectangle){0, 0, 64.0f, 64.0f}, (Rectangle){body.Position.x, body.Position.y, shape->width, shape->width}, (Vector2){half_width, half_width}, -body.Rotation * RAD2DEG, WHITE);
    }
}

void Draw::Circle(const Body &body) {
    const float radius = dynamic_cast<CircleShape *>(body.shape.get())->Radius;
    const float a = glm::mod(body.Rotation * RAD2DEG, 360.0f);

    const Color color = body.IsColliding ? RED : WHITE;
    DrawCircleSectorLines(Vector2(body.Position.x, body.Position.y), radius, a, a + 360.0f, 0, color);
}

void Draw::Polygon(const Body &body) {
    const Color col = body.IsColliding ? RED : WHITE;
    DrawPolygon(dynamic_cast<PolygonShape *>(body.shape.get())->Vertices, body.Position, body.Rotation, col);
}