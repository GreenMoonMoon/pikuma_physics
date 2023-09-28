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
//        particles.emplace_back(25, 150, 0.5);
//        particles.emplace_back(100, 160, 3.0);
//        particles.emplace_back(45, 170, 4.0);
//        particles.emplace_back(75, 180, 5.0);

        push = vec2(0.0f);

//        // Liquid rectangle
//        liquid.x = 0;
//        liquid.y = screenHeight / 2;
//        liquid.width = screenWidth;
//        liquid.height = screenHeight / 2;
    }

    void Input() {
        push = vec2(0.0f);
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
        const glm::vec2 wind = glm::vec2(1.0f, 0.0f) * PIXEL_PER_METER;
        const vec2 gravity = vec2(0.0f, 9.8f) * PIXEL_PER_METER;
        for (auto &particle: particles) {
            // Adding push force
            particle.AddForce(push);

            // Applying friction
            glm::vec2 frictionForce = Force::GenerateFrictionForce(particle.Velocity, 10.0f * PIXEL_PER_METER);
            particle.AddForce(frictionForce);

//            // Applying forces depending on the liquid
//            // Adding wind force
//            // Adding Drag force
//            vec2 dragForce;
//            if (particle.Position.y > liquid.height) {
//                dragForce = Force::GenerateDragForce(particle.Velocity, 0.05f);
//                particle.AddForce(dragForce);
//            } else {
//                particle.AddForce(wind);
//            }
//
//            // Adding Gravity (Weight Force)
//            particle.AddForce(gravity * particle.Mass);
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
