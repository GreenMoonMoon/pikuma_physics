#include <vector>
#include <iostream>

#include "physics/body.h"
#include "physics/physic_constants.h"
#include "physics/forces.h"
#include "physics/collisions.h"
#include "graphics/draw.h"
#include "raylib.h"

static glm::vec2 push;
static Rectangle liquid;
static float springRestDistance;
static float springStiffness;

const int screenWidth = 840;
const int screenHeight = 680;

// APPLICATION
class Application {
private:
    bool running;
    std::vector<Body> bodies;
    std::vector<Contact> contacts;

public:
    Application() : running(true), bodies({}) {
        SetConfigFlags(FLAG_MSAA_4X_HINT);
        SetConfigFlags(FLAG_WINDOW_RESIZABLE);
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
        bodies.emplace_back(CircleShape(25.0f), screenWidth / 2 + 5, 100, 1.0f, 0.0f);
        bodies.emplace_back(CircleShape(50.0f), screenWidth / 2, 300, 0.0f, 0.0f);
        bodies.emplace_back(CircleShape(45.0f), screenWidth / 2 - 75, 200, 3.0f, 0.0f);
        bodies.emplace_back(CircleShape(35.0f), screenWidth / 2 + 50, 200, 1.5f, 0.0f);
//        bodies.emplace_back(BoxShape(100.0f, 100.0f), screenWidth / 2, screenHeight / 2, 1.0f, 0.0f);
//        bodies.emplace_back(BoxShape(100.0f, 100.0f), 300, screenHeight / 2, 1.0f, 0.0f);

        push = glm::vec2(0.0f);

        springRestDistance = 80.0f;
        springStiffness = 500.0f;
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

//            // DRAG
//            glm::vec2 dragForce = Force::GenerateDragForce(body, 0.001f);
//            body.AddForce(dragForce);

            // WIND
            static glm::vec2 windForce(10.0f, 0.0f);
            body.AddForce(windForce);

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
                    body.Velocity.x *= -0.9f;
                }
                if (body.Position.x < radius) {
                    body.Position.x = radius;
                    body.Velocity.x *= -0.9f;
                }
                if (body.Position.y > heightBoundary) {
                    body.Position.y = heightBoundary;
                    body.Velocity.y *= -0.9f;
                }
                if (body.Position.y < radius) {
                    body.Position.y = radius;
                    body.Velocity.y *= -0.9f;
                }
            }
        }

        // Body collisions
        for (auto &body: bodies) {
            body.IsColliding = false;
        }
        contacts.clear();

        for (int i = 0; i < bodies.size() - 1; ++i) {
            for (int j = i + 1; j < bodies.size(); ++j) {

                Contact contact{};
                if(CollisionDetection::IsColliding(bodies[i], bodies[j], contact)) {
                    bodies[j].IsColliding = true;
                    bodies[i].IsColliding = true;

//                    contacts.push_back(contact);
                    contact.ResolvePenetration();
                }
            }
        }
    }

    void Render() {
        ClearBackground(BLACK);
        BeginDrawing();
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, DARKGREEN, Color(0, 35, 5, 255));

        // Draw shapes
        for (auto &body: bodies) {
            if (body.shape->GetType() == CIRCLE) {
                Draw::ShadedCircle(body);
            } else if (body.shape->GetType() == POLYGON || body.shape->GetType() == BOX) {
                Draw::Polygon(body);
            }
        }

        for (auto &body: bodies) {
            if (body.shape->GetType() == CIRCLE) {
                Draw::Circle(body);
            } else if (body.shape->GetType() == POLYGON || body.shape->GetType() == BOX) {
                Draw::Polygon(body);
            }
        }

        for (auto contact: contacts) {
            DrawCircle(contact.start[0], contact.start[1], 2, ORANGE);
            DrawCircle(contact.end[0], contact.end[1], 2, ORANGE);
            DrawLine(contact.start[0], contact.start[1], contact.end[0], contact.end[1], ORANGE);
        }

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
