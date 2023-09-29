#include "raylib.h"
#include "particles.h"
#include "physic_constants.h"
#include "forces.h"
#include "glm/vec2.hpp"
#include <vector>


static glm::vec2 push;
static Rectangle liquid;
static float springRestDistance;
static float springStiffness;

const float pointRadius = 8.0;
const int screenWidth = 840;
const int screenHeight = 680;

class Application {
private:
    bool running;
    std::vector<Particle> particles;

    float elapsed_time;

public:
    Application() : running(true), particles({}), elapsed_time(0.0f) {
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
        particles.emplace_back(50, 100, 1.0f);

        push = glm::vec2(0.0f);

//        // Liquid rectangle
//        liquid.x = 0;
//        liquid.y = screenHeight / 2;
//        liquid.width = screenWidth;
//        liquid.height = screenHeight / 2;

        springRestDistance = 10.0f;
        springStiffness = 100.0f;
    }

    void Input() {
        push = glm::vec2(0.0f);
        if(IsKeyDown(KEY_S)) push.y += 15.0f * PIXEL_PER_METER;
        if(IsKeyDown(KEY_W)) push.y -= 15.0f * PIXEL_PER_METER;
        if(IsKeyDown(KEY_D)) push.x += 15.0f * PIXEL_PER_METER;
        if(IsKeyDown(KEY_A)) push.x -= 15.0f * PIXEL_PER_METER;

        if(IsKeyPressed(KEY_UP)) springRestDistance += 0.5f;
        if(IsKeyPressed(KEY_DOWN)) springRestDistance -= 0.5f;
        if(IsKeyPressed(KEY_RIGHT)) springRestDistance += 5.0f;
        if(IsKeyPressed(KEY_LEFT)) springRestDistance -= 5.0f;

//        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            particles.emplace_back(GetMousePosition().x, GetMousePosition().y, 1.0);
        }

        PollInputEvents();
    }

    void Update() {
        // Adding forces
        for (auto &particle: particles) {
            // Applying friction
            glm::vec2 frictionForce = Force::GenerateFrictionForce(particle, 10.0f * PIXEL_PER_METER);
            particle.AddForce(frictionForce);

            glm::vec2 springForce = Force::GenerateSpringForce(
                    particle,
                    glm::vec2(screenWidth * 0.5f, screenHeight * 0.5f),
                    springRestDistance * PIXEL_PER_METER,
                    springStiffness
            );
            particle.AddForce(springForce);
        }

        // Force integration
        for (auto &particle: particles) {
            particle.Integrate(GetFrameTime());
        }

        // Boundary collisions
        for (auto &particle: particles) {
            float widthBoundary = screenWidth - pointRadius;
            float heightBoundary = screenHeight - pointRadius;
            if (particle.Position.x > widthBoundary) {
                particle.Position.x = widthBoundary;
                particle.Velocity.x *= -0.9f;
            }
            if (particle.Position.x < pointRadius) {
                particle.Position.x = pointRadius;
                particle.Velocity.x *= -0.9f;
            }
            if (particle.Position.y > heightBoundary) {
                particle.Position.y = heightBoundary;
                particle.Velocity.y *= -0.9f;
            }
            if (particle.Position.y < pointRadius) {
                particle.Position.y = pointRadius;
                particle.Velocity.y *= -0.9f;
            }
        }
    }

    void Render() {
        ClearBackground(DARKGREEN);
        BeginDrawing();

        DrawRectangleRec(liquid, BLUE);

        for (auto &particle: particles) {
            DrawCircle((int)particle.Position.x, (int)particle.Position.y, pointRadius, WHITE);
        }

        EndDrawing();
    }

    void Cleanup() {
        particles.clear();
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
