#include <vector>
#include <iostream>

#include "physics/body.h"
#include "physics/physic_constants.h"
#include "physics/forces.h"
#include "physics/collisions.h"
#include "graphics/draw.h"
#include "raylib.h"
#include "raymath.h"
#include "glm/vec2.hpp"
#include "glm/geometric.hpp"

static glm::vec2 push;
static float springRestDistance;
static float springStiffness;

const int screenWidth = 840;
const int screenHeight = 680;

const int radius1 = 25.0;
const int radius2 = 35.0;
const int radius3 = 50.0;
const int radius4 = 71.0;
const int radius5 = 100.0;
const int radius6 = 141.0;
const int radius7 = 200.0;


static struct {
    bool IsActive;
    Vector2 Position;
    float Radius;
    float Mass;
    int type;
} spawnShape;

// APPLICATION
class Application {
private:
    bool running;
    std::vector<Body> bodies;
    std::vector<Contact> contacts;

public:
    Application() : running(true), bodies({}) {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        InitWindow(screenWidth, screenHeight, "Pikuma Physics");

        SetTargetFPS(60.0f);

        Setup();
    }

    ~Application() {
        Cleanup();

        CloseWindow();
    }

    inline bool IsRunning() const {
        return running and not WindowShouldClose();
    }

    void Setup() {
        bodies.emplace_back(CircleShape(radius1), screenWidth / 2 + 5, 100, 2.0f, 0.0f, 0);
        bodies.emplace_back(CircleShape(radius2), screenWidth / 2, 300, 4.0f, 0.0f, 1);
        bodies.emplace_back(CircleShape(radius3), screenWidth / 2 - 75, 200, 8.0f, 0.0f, 2);
        bodies.emplace_back(CircleShape(radius4), screenWidth / 2 + 50, 200, 16.0f, 0.0f, 3);
    }

    void Input() {
        push = glm::vec2(0.0f);
        if (IsKeyDown(KEY_S)) push.y += 15.0f * PIXEL_PER_METER;
        if (IsKeyDown(KEY_W)) push.y -= 15.0f * PIXEL_PER_METER;
        if (IsKeyDown(KEY_D)) push.x += 15.0f * PIXEL_PER_METER;
        if (IsKeyDown(KEY_A)) push.x -= 15.0f * PIXEL_PER_METER;

        if (IsKeyDown(KEY_UP)) springRestDistance += 5.0f;
        if (IsKeyDown(KEY_DOWN)) springRestDistance -= 5.0f;
        if (IsKeyDown(KEY_RIGHT)) springStiffness += 2.0f;
        if (IsKeyDown(KEY_LEFT)) springStiffness -= 2.0f;

        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
            spawnShape.Position = GetMousePosition();
            spawnShape.Mass = 0.0f;
            spawnShape.Radius = 0.0f;
            spawnShape.IsActive = true;
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) and spawnShape.IsActive) {
            spawnShape.Radius = 25.0f;
            spawnShape.Mass = 2.0f;
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) and spawnShape.IsActive) {
            bodies.emplace_back(CircleShape(spawnShape.Radius), spawnShape.Position.x, spawnShape.Position.y,
                                spawnShape.Mass, 0.0f, 0);
            spawnShape.IsActive = false;
        }

        PollInputEvents();
    }

    void Update() {
        // Adding forces
        for (auto &body: bodies) {
//            // FRICTION
//            glm::vec2 frictionForce = Force::GenerateFrictionForce(body, 1.0f * PIXEL_PER_METER);
//            body.AddForce(frictionForce);

//            // PUSH
//            body.AddForce(push);

            // DRAG
            glm::vec2 dragForce = Force::GenerateDragForce(body, 0.001f);
            body.AddForce(dragForce);

//            // WIND
//            static glm::vec2 windForce(10.0f, 0.0f);
//            body.AddForce(windForce);

            // WEIGHT
            glm::vec2 weightForce(0.0f, body.Mass * 9.8f * PIXEL_PER_METER);
            body.AddForce(weightForce);

//            // TORQUE
//            float torqueForce = 200.0f;
//            body.AddTorque(torqueForce);
        }

        // Force integration
        for (auto &body: bodies) {
            body.Update(GetFrameTime());
        }

        // COLLISIONS
        // Boundary collisions
        for (auto &body: bodies) {
            if (body.shape->GetType() == CIRCLE) {
                float radius = dynamic_cast<CircleShape *>(body.shape.get())->Radius;
                float widthBoundary = screenWidth - radius;
                float heightBoundary = screenHeight - radius;
                if (body.Position.x > widthBoundary) {
                    body.Position.x = widthBoundary;
                    body.Velocity.x *= -body.restitution;
                }
                if (body.Position.x < radius) {
                    body.Position.x = radius;
                    body.Velocity.x *= -body.restitution;
                }
                if (body.Position.y > heightBoundary) {
                    body.Position.y = heightBoundary;
                    body.Velocity.y *= -body.restitution;
                }
                if (body.Position.y < radius) {
                    body.Position.y = radius;
                    body.Velocity.y *= -body.restitution;
                }
            }
        }

        // Body collisions
        // Resets bodies collisions
        for (auto &body: bodies) {
            body.IsColliding = false;
        }
        contacts.clear();

        for (int i = 0; i < bodies.size() - 1; ++i) {
            for (int j = i + 1; j < bodies.size(); ++j) {

                Contact contact{};
                if (Collision::IsColliding(bodies[i], bodies[j], contact)) {
                    bodies[j].IsColliding = true;
                    bodies[i].IsColliding = true;

                    if (bodies[j].BallType == bodies[i].BallType) {
//                        Collision::ResolveMerge(contact);
                        switch (bodies[j].BallType) {
                            case 0:
                                spawnShape.Radius = radius2;
                                spawnShape.Mass = 4.0f;
                                spawnShape.type = 1;
                                break;
                            case 1:
                                spawnShape.Radius = radius3;
                                spawnShape.Mass = 8.0f;
                                spawnShape.type = 2;
                                break;
                            case 2:
                                spawnShape.Radius = radius4;
                                spawnShape.Mass = 16.0f;
                                spawnShape.type = 3;
                                break;
                            case 3:
                                spawnShape.Radius = radius5;
                                spawnShape.Mass = 32.0f;
                                spawnShape.type = 4;
                            break;
                            case 4:
                                spawnShape.Radius = radius6;
                                spawnShape.Mass = 64.0f;
                                spawnShape.type = 5;
                                break;
                            case 5:
                                spawnShape.Radius = radius7;
                                spawnShape.Mass = 128.0f;
                                spawnShape.type = 6;
                                break;
                            default:
                                break;
                        }
                        spawnShape.Position = Vector2(0.5f * (contact.end.x + contact.start.x),
                                                      0.5f * (contact.end.y + contact.start.y));

                        glm::vec2 velocity = bodies[j].Velocity + bodies[i].Velocity;

                        bodies.erase(bodies.begin()+j);
                        bodies.erase(bodies.begin()+i);

                        bodies.emplace_back(CircleShape(spawnShape.Radius), spawnShape.Position.x,
                                            spawnShape.Position.y, spawnShape.Mass, 0.0f, spawnShape.type);
                        bodies.back().Velocity = velocity;
                    } else {
                        Collision::ResolveCollision(contact);
                    }
                }
            }
        }
    }

    void Render() {
        ClearBackground(BLACK);
        BeginDrawing();
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, DARKGREEN, Color(0, 35, 5, 255));

        // Draw Container
        DrawRectangleLines(50, 50, 250, 450, BLACK);

        // Draw shapes
        for (auto &body: bodies) {
            if (body.shape->GetType() == CIRCLE) {
                Draw::ShadedCircle(body);
            } else if (body.shape->GetType() == POLYGON || body.shape->GetType() == BOX) {
                Draw::Polygon(body);
            }
        }

//        for (auto &body: bodies) {
//            if (body.shape->GetType() == CIRCLE) {
//                Draw::Circle(body);
//            } else if (body.shape->GetType() == POLYGON || body.shape->GetType() == BOX) {
//                Draw::Polygon(body);
//            }
//        }

//        for (auto contact: contacts) {
//            DrawCircle(contact.start[0], contact.start[1], 2, ORANGE);
//            DrawCircle(contact.end[0], contact.end[1], 2, ORANGE);
//            DrawLine(contact.start[0], contact.start[1], contact.end[0], contact.end[1], ORANGE);
//        }

//        if (spawnShape.IsActive){
//            DrawCircleLines(spawnShape.Position.x, spawnShape.Position.y, spawnShape.Radius, LIGHTGRAY);
//            DrawText(TextFormat("MASS: %0g", spawnShape.Mass),
//                     spawnShape.Position.x - 16.0f,
//                     spawnShape.Position.y + spawnShape.Radius + 5.0f,
//                     16,
//                     LIGHTGRAY);
//        }

        DrawFPS(10, 10);

        EndDrawing();
    }

    void Cleanup() {
        bodies.clear();
    }
};

int main() {
    Application app;

    while (app.IsRunning()) {
        app.Input();
        app.Update();
        app.Render();
    }

    return 0;
}
