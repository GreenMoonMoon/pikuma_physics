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

constexpr int screenWidth = 840;
constexpr int screenHeight = 680;

static struct {
    bool IsActive;
    Vector2 Position;
    float Radius;
    float Mass;
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

        Draw::Init();

        Setup();
    }

    ~Application() {
        Cleanup();

        CloseWindow();
    }

    bool IsRunning() const {
        return running and not WindowShouldClose();
    }

    void Setup() {
        bodies.emplace_back(BoxShape(100.0f, 100.0f), screenWidth / 2, screenHeight / 2, 1.0f, 0.0f);
        bodies.emplace_back(BoxShape(100.0f, 100.0f), 400,  screenHeight / 2, 1.0f, 0.0f);
        // bodies[0].AngularVelocity = 0.4f;
        // bodies[1].AngularVelocity = -0.25f;

        push = glm::vec2(0.0f);

        springRestDistance = 80.0f;
        springStiffness = 500.0f;
    }

    void Input() {
//        push = glm::vec2(0.0f);
//        if (IsKeyDown(KEY_S)) push.y += 15.0f * PIXEL_PER_METER;
//        if (IsKeyDown(KEY_W)) push.y -= 15.0f * PIXEL_PER_METER;
//        if (IsKeyDown(KEY_D)) push.x += 15.0f * PIXEL_PER_METER;
//        if (IsKeyDown(KEY_A)) push.x -= 15.0f * PIXEL_PER_METER;
//
//        if (IsKeyDown(KEY_UP)) springRestDistance += 5.0f;
//        if (IsKeyDown(KEY_DOWN)) springRestDistance -= 5.0f;
//        if (IsKeyDown(KEY_RIGHT)) springStiffness += 2.0f;
//        if (IsKeyDown(KEY_LEFT)) springStiffness -= 2.0f;

//        if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
//            spawnShape.Position = GetMousePosition();
//            spawnShape.Mass = 0.0f;
//            spawnShape.Radius = 0.0f;
//            spawnShape.IsActive = true;
//        }
//        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) and spawnShape.IsActive){
//            auto offset = GetMousePosition();
//            spawnShape.Radius = Vector2Distance(offset, spawnShape.Position);
//            spawnShape.Mass = abs(spawnShape.Position.y - offset.y);
//        }
//        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) and spawnShape.IsActive) {
//            bodies.emplace_back(CircleShape(spawnShape.Radius), spawnShape.Position.x, spawnShape.Position.y, spawnShape.Mass, 0.0f);
//            spawnShape.IsActive = false;
//        }

        PollInputEvents();
    }

    void Update() {
        // Moving box a according to the mouse
        glm::vec2 mouse_input = {GetMousePosition().x, GetMousePosition().y};
        bodies[0].Position = mouse_input;

        // Adding forces
        for (auto &body: bodies) {
//            // FRICTION
//            glm::vec2 frictionForce = Force::GenerateFrictionForce(body, 1.0f * PIXEL_PER_METER);
//            body.AddForce(frictionForce);
//
//            // PUSH
//            body.AddForce(push);

            // DRAG
            glm::vec2 dragForce = Force::GenerateDragForce(body, 0.001f);
            body.AddForce(dragForce);

            // // WIND
            // static glm::vec2 windForce(10.0f, 0.0f);
            // body.AddForce(windForce);

            // // WEIGHT
            // glm::vec2 weightForce(0.0f, body.Mass * 9.8f * PIXEL_PER_METER);
            // body.AddForce(weightForce);

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
        // TODO: move into collision.h
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
            else if (body.shape->GetType() == BOX) {
                BoxShape *shape = dynamic_cast<BoxShape *>(body.shape.get());
                float widthBoundary = screenWidth - shape->width * 0.5f;
                float heightBoundary = screenHeight - shape->height * 0.5f;
//                if (body.Position.x < )
            }
        }

        // Body collisions
        // TODO: move to its own method
        for (auto &body: bodies) {
            body.IsColliding = false;

            if(body.shape->GetType() == POLYGON || body.shape->GetType() == BOX) {
                dynamic_cast<PolygonShape*>(body.shape.get())->UpdateWorldVertices(body.Position, body.Rotation);
            }
        }
        contacts.clear();

        for (int i = 0; i < bodies.size() - 1; ++i) {
            for (int j = i + 1; j < bodies.size(); ++j) {
                if(Contact contact{}; Collision::IsColliding(bodies[i], bodies[j], contact)) {
                    bodies[j].IsColliding = true;
                    bodies[i].IsColliding = true;

                    // TODO: reenable
                    // Collision::ResolveCollision(contact);

                    contacts.push_back(contact);
                }
            }
        }
    }

    void Render() {
        ClearBackground(BLACK);
        BeginDrawing();
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, DARKGREEN, Color(0, 35, 5, 255));

        // Draw shapes
        for (auto&body: bodies) {
            if (body.shape->GetType() == CIRCLE) {
                Draw::ShadedCircle(body);
            }
            else if (body.shape->GetType() == BOX) {
                Draw::ShadedBox(body);
            }
            else if (body.shape->GetType() == POLYGON) {
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

        // UI
        // // Draw spawn shape
        // if (spawnShape.IsActive){
        //     DrawCircleLines(spawnShape.Position.x, spawnShape.Position.y, spawnShape.Radius, LIGHTGRAY);
        //     DrawText(TextFormat("MASS: %0g", spawnShape.Mass),
        //              spawnShape.Position.x - 16.0f,
        //              spawnShape.Position.y + spawnShape.Radius + 5.0f,
        //              16,
        //              LIGHTGRAY);
        // }

        DrawFPS(10, 10);

        EndDrawing();
    }

    void Cleanup() {
        Draw::Cleanup();
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
