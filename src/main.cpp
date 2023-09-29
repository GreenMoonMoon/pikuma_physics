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
        particles.emplace_back(screenWidth / 2, 100, 1.0f);
        particles.emplace_back(screenWidth / 2, 120, 1.0f);
        particles.emplace_back(screenWidth / 2, 140, 1.0f);
        particles.emplace_back(screenWidth / 2, 160, 1.0f);
        particles.emplace_back(screenWidth / 2, 180, 1.0f);
        particles.emplace_back(screenWidth / 2, 200, 1.0f);

        push = glm::vec2(0.0f);

//        // Liquid rectangle
//        liquid.x = 0;
//        liquid.y = screenHeight / 2;
//        liquid.width = screenWidth;
//        liquid.height = screenHeight / 2;

        springRestDistance = 80.0f;
        springStiffness = 500.0f;
    }

    void Input() {
        push = glm::vec2(0.0f);
        if(IsKeyDown(KEY_S)) push.y += 15.0f * PIXEL_PER_METER;
        if(IsKeyDown(KEY_W)) push.y -= 15.0f * PIXEL_PER_METER;
        if(IsKeyDown(KEY_D)) push.x += 15.0f * PIXEL_PER_METER;
        if(IsKeyDown(KEY_A)) push.x -= 15.0f * PIXEL_PER_METER;

        if(IsKeyDown(KEY_UP)) springRestDistance += 5.0f;
        if(IsKeyDown(KEY_DOWN)) springRestDistance -= 5.0f;
        if(IsKeyDown(KEY_RIGHT)) springStiffness += 2.0f;
        if(IsKeyDown(KEY_LEFT)) springStiffness -= 2.0f;

//        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            particles.emplace_back(GetMousePosition().x, GetMousePosition().y, 1.0);
        }

        PollInputEvents();
    }

    void Update() {
        // Adding forces
        for (auto &particle: particles) {
            // FRICTION
            glm::vec2 frictionForce = Force::GenerateFrictionForce(particle, 1.0f * PIXEL_PER_METER);
            particle.AddForce(frictionForce);

            // PUSH
            particle.AddForce(push);

            // DRAG
            glm::vec2 dragForce = Force::GenerateDragForce(particle, 0.001f);
            particle.AddForce(dragForce);

            // WEIGHT
            glm::vec2 weightForce(0.0f, particle.Mass * 9.8f * PIXEL_PER_METER);
            particle.AddForce(weightForce);
        }


        // SPRING

        glm::vec2 sp = Force::GenerateSpringForce(
                particles[0],
                glm::vec2(screenWidth * 0.5f, 50.0f),
                springRestDistance,
                springStiffness
        );
        particles[0].AddForce(sp);
        for (int i = 1; i < particles.size(); ++i) {
            sp = Force::GenerateSpringForce(particles[i], particles[i - 1], springRestDistance, springStiffness);
            particles[i].AddForce(sp);
            particles[i -1].AddForce(-sp);
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

        DrawLine((int)particles[0].Position.x,
                 (int)particles[0].Position.y,
                 screenWidth / 2,
                 50,
                 RAYWHITE);
        for (int i = 1; i < particles.size(); ++i) {
            DrawLine((int)particles[i].Position.x,
                     (int)particles[i].Position.y,
                     (int)particles[i - 1].Position.x,
                     (int)particles[i - 1].Position.y,
                     RAYWHITE);
        }

        DrawFPS(10, 10);

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
