#include "raylib.h"
#include "particles.h"
#include "physic_constants.h"
#include "forces.h"
#include "glm/vec2.hpp"
#include <vector>


static glm::vec2 push;
static Rectangle liquid;

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
        particles.emplace_back(50, 100, 1.0);
        particles.emplace_back(100, 150, 100.0);

        push = glm::vec2(0.0f);

//        // Liquid rectangle
//        liquid.x = 0;
//        liquid.y = screenHeight / 2;
//        liquid.width = screenWidth;
//        liquid.height = screenHeight / 2;
    }

    void Input() {
        push = glm::vec2(0.0f);
        if(IsKeyDown(KEY_S)) push.y += 15.0f * PIXEL_PER_METER;
        if(IsKeyDown(KEY_W)) push.y -= 15.0f * PIXEL_PER_METER;
        if(IsKeyDown(KEY_D)) push.x += 15.0f * PIXEL_PER_METER;
        if(IsKeyDown(KEY_A)) push.x -= 15.0f * PIXEL_PER_METER;

//        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            particles.emplace_back(GetMousePosition().x, GetMousePosition().y, 1.0);
        }

        PollInputEvents();
    }

    void Update() {
        // Adding forces
        for (auto &particle: particles) {
            particle.ClearForces();
            // Adding push force
            particle.AddForce(push);

            // Applying friction
//            glm::vec2 frictionForce = Force::GenerateFrictionForce(particle.Velocity, 10.0f * PIXEL_PER_METER);
//            particle.AddForce(frictionForce);
        }

        // Applying gravitational force
        glm::vec2 gravitationalForce = Force::GenerateGravitationalForce(particles[0], particles[1], 10000.0f * PIXEL_PER_METER);
        particles[0].AddForce(gravitationalForce);
        particles[1].AddForce(-gravitationalForce);

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

//        for (auto &particle: particles) {
//            DrawCircle((int)particle.Position.x, (int)particle.Position.y, pointRadius, WHITE);
//        }
        DrawCircle((int)particles[0].Position.x, (int)particles[0].Position.y, pointRadius, RED);
        DrawLine((int)particles[0].Position.x,
                 (int)particles[0].Position.y,
                 (int)(particles[0].Position.x + particles[0].TotalForce.x * 0.1f),
                 (int)(particles[0].Position.y + particles[0].TotalForce.y * 0.1f),
                 RED);

        DrawCircle((int)particles[1].Position.x, (int)particles[1].Position.y, pointRadius, BLUE);
        DrawLine((int)particles[1].Position.x,
                 (int)particles[1].Position.y,
                 (int)(particles[1].Position.x + particles[1].TotalForce.x * 0.1f),
                 (int)(particles[1].Position.y + particles[1].TotalForce.y * 0.1f),
                 BLUE);

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
