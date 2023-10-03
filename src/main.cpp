#include "raylib.h"
#include "body.h"
#include "physic_constants.h"
#include "forces.h"
#include "glm/vec2.hpp"
#include "glm/gtx/rotate_vector.hpp"
#include <vector>

static glm::vec2 push;
static Rectangle liquid;
static float springRestDistance;
static float springStiffness;

const int screenWidth = 840;
const int screenHeight = 680;

class Application {
private:
    bool running;
    std::vector<Body> bodies;

    float elapsed_time;

public:
    Application() : running(true), bodies({}), elapsed_time(0.0f) {
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
        bodies.emplace_back(CircleShape(50.0f), screenWidth / 2, 100, 1.0f);

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
            // FRICTION
            glm::vec2 frictionForce = Force::GenerateFrictionForce(body, 1.0f * PIXEL_PER_METER);
            body.AddForce(frictionForce);

            // PUSH
            body.AddForce(push);

            // DRAG
            glm::vec2 dragForce = Force::GenerateDragForce(body, 0.001f);
            body.AddForce(dragForce);

            // WEIGHT
            glm::vec2 weightForce(0.0f, body.Mass * 9.8f * PIXEL_PER_METER);
            body.AddForce(weightForce);
        }

        // Force integration
        for (auto &body: bodies) {
            body.Integrate(GetFrameTime());
        }

        // Boundary collisions
        for (auto &body: bodies) {
            if (body.shape->GetType() == CIRCLE) {
                float radius = dynamic_cast<CircleShape *>(body.shape)->radius;
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
    }

    void Render() {
        ClearBackground(DARKGREEN);
        BeginDrawing();

        static float angle = 0.0f;

        // Draw Circle shape
        for (auto &body: bodies) {
            if (body.shape->GetType() == CIRCLE) {
                float radius = dynamic_cast<CircleShape *>(body.shape)->radius;

                DrawCircleLines((int) body.Position.x,
                                (int) body.Position.y,
                                radius,
                                WHITE);
                glm::vec2 line = glm::vec2(radius, 0.0f);
                line = glm::rotate(line, angle);
                line += body.Position;
                DrawLine((int) body.Position.x,
                         (int) body.Position.y,
                         (int) line.x,
                         (int) line.y,
                         WHITE);

            }
        }

        DrawFPS(10, 10);

        EndDrawing();

        angle += 0.01f;
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
