#include <vector>
#include <iostream>
#include <glm/detail/func_geometric.inl>

#include "physics/body.h"
#include "physics/physic_constants.h"
#include "physics/forces.h"
#include "physics/collisions.h"

#include "graphics/draw.h"
#include "raylib.h"
#define RAYGUI_IMPLEMENTATION
#include "raygui.h"
#include "raymath.h"

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

class DebugWindow {
private:
    bool show;
    Rectangle window;
    bool grabbing;
    Vector2 grab_offset;
public:
    DebugWindow() {
        show = true;
        window = (Rectangle){10.0f, 10.0f, 200.0f, 500.0f};
        grabbing = false;
        grab_offset = (Vector2){0};
    }
    ~DebugWindow() = default;

    void Inputs() {
        if (IsKeyPressed(KEY_GRAVE)) { show = true; }
        if (!show) { return; }
        if (CheckCollisionPointRec(GetMousePosition(), (Rectangle){window.x, window.y, window.width, 20})) {
            if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) && !grabbing) {
                grabbing = true;
                grab_offset = Vector2Subtract(GetMousePosition(), (Vector2){window.x, window.y});
            }
        }
        if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT) && grabbing) {
            grabbing = false;
        }
    }

    void Update() {
        if (!show) { return; }
        if (grabbing) {
            window.x = GetMousePosition().x - grab_offset.x;
            window.y = GetMousePosition().y - grab_offset.y;
        }
    }

    void Draw() {
        if (!show) { return; }
        if (!GuiWindowBox(window, "Debug")) {
            if (GuiButton((Rectangle){window.x + 10, window.y + 35, 100.0f, 25.0f}, "Box")) {

            }
        } else {
            show = false;
        }
    }
};

DebugWindow debug_window;

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
        bodies[0].Rotation = 1.0f;
        bodies[1].Rotation = 0.5f;

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
        debug_window.Inputs();

        //PollInputEvents();
    }

    void Update() {
        debug_window.Update();

        bodies[0].Position = {GetMousePosition().x, GetMousePosition().y};

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
                const float radius = dynamic_cast<CircleShape *>(body.shape.get())->Radius;
                const float widthBoundary = screenWidth - radius;
                const float heightBoundary = screenHeight - radius;
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
                const BoxShape *shape = dynamic_cast<BoxShape *>(body.shape.get());
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

                    // Collision::ResolveCollision(contact);

                    contacts.push_back(contact);
                }
            }
        }
    }

    void Render() {
        ClearBackground(BLACK);
        BeginDrawing();

        // Background
        DrawRectangleGradientV(0, 0, screenWidth, screenHeight, DARKGREEN, Color(0, 35, 5, 255));

        // Shapes
        for (auto&body: bodies) {
            switch (body.shape->GetType()) {
                case CIRCLE:
                    Draw::ShadedCircle(body); break;
                case BOX:
                    Draw::ShadedBox(body); break;
                case POLYGON:
                    Draw::Polygon(body); break;
                default: break;
            }
        }

        // Outlines
        for (auto &body: bodies) {
            switch (body.shape->GetType()) {
            case CIRCLE:
                Draw::Circle(body); break;
            case BOX:
            case POLYGON:
                Draw::Polygon(body); break;
            }
        }

        // Debug draw contacts
        for (auto contact: contacts) {
            DrawRectangle(contact.start.x - 2, contact.start.y - 2, 4, 4, ORANGE);
            DrawText("A", contact.a->Position.x, contact.a->Position.y, 28, WHITE);
            DrawRectangle(contact.end.x - 2, contact.end.y - 2, 4, 4, ORANGE);
            DrawText("B", contact.b->Position.x, contact.b->Position.y, 28, WHITE);
            const glm::vec2 line_end = contact.start + contact.normal * 10.0f;
            // draw contact normal
            DrawLine(contact.start[0], contact.start[1], line_end.x, line_end.y, ORANGE);
        }

        debug_window.Draw();

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
