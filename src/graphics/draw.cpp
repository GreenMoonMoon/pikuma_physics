//
// Created by josue on 2023-10-17.
//

#include "draw.h"
#include "physics/shape.h"
#include "raylib.h"
#include "raylib_extension.h"
#include "glm/ext/scalar_common.hpp"

void Draw::ShadedCircle(const Body &body) {
    float radius = dynamic_cast<CircleShape *>(body.shape.get())->Radius;
    switch (body.BallType) {
        case 0:
            DrawCircleGradient(body.Position.x, body.Position.y, radius, WHITE, BLUE);
            break;
        case 1:
            DrawCircleGradient(body.Position.x, body.Position.y, radius, BLUE, DARKBLUE);
            break;
        case 2:
            DrawCircleGradient(body.Position.x, body.Position.y, radius, DARKBLUE, PURPLE);
            break;
        case 3:
            DrawCircleGradient(body.Position.x, body.Position.y, radius, PURPLE, DARKPURPLE);
            break;
        default:
            DrawCircleGradient(body.Position.x, body.Position.y, radius, DARKPURPLE, BROWN);
            break;
    }

//    if (body.IsColliding){
//        DrawCircleGradient(body.Position.x, body.Position.y, radius, RED, DARKBROWN);
//    } else {
//        DrawCircleGradient(body.Position.x, body.Position.y, radius, BLUE, DARKBLUE);
//    }
}

void Draw::Circle(const Body &body) {
    float radius = dynamic_cast<CircleShape *>(body.shape.get())->Radius;
    float a = glm::mod(body.Rotation * RAD2DEG, 360.0f);

    Color col = body.IsColliding ? RED : WHITE;
    DrawCircleSectorLines(Vector2(body.Position.x, body.Position.y), radius, a, a + 360.0f, 0, col);
}

void Draw::Polygon(Body &body) {
    Color col = body.IsColliding ? RED : WHITE;
    DrawPolygon(dynamic_cast<PolygonShape *>(body.shape.get())->Vertices, body.Position, body.Rotation, col);
}